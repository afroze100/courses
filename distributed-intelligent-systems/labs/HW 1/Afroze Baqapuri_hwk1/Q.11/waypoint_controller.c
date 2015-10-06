/*
 * File:         waypoint_controller.c
 * Date:         October 6th, 2012
 * Description:  A controller driving a robot through a list of waypoints.
 * Author:       Adrian Arfire
 *
 */

#include <webots/robot.h>
#include <webots/differential_wheels.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <webots/emitter.h>
#include <webots/receiver.h>

#define MAX_SPEED 1000.0 	// Maximum speed of wheels in each direction
#define MAX_ACC 1000.0 		// Maximum amount speed can change in 128 ms
#define NB_WAYPOINTS 5 	// Number of waypoints (THIS NEEDS TO BE SET ALSO IN pso.h and pso_obs_sup.c)
#define NB_SENSOR 8 		// Number of proximity sensors
#define DATASIZE 2*NB_WAYPOINTS // Number of elements in particle

// Odometry definitions
#define AXLE_LENGTH          0.052     // Distance between wheels of robot
#define ENCODER_RES      	 159.23    // Encoder resolution
#define WHEEL_RADIUS         0.0205    // Wheel radius in meters
#define DELTA_T              0.064     // Time step in seconds

WbDeviceTag emitter;
WbDeviceTag rec;
double gwaypoints[DATASIZE];

enum movState {
	ALIGN,
	TRANS
};

unsigned char loc_state = ALIGN;
double fitness_best = 0.0;

double fitfunc(double[],int);

void reset(void) {
    int i;
    char text[4];
    text[1]='s';
    text[3]='\0';
    emitter = wb_robot_get_device("emitter");
    rec = wb_robot_get_device("receiver");
}

int main() {
    double buffer[255];
    double fit;
    double *rbuffer;
    int i;

    wb_robot_init();
    reset();

    receiver_enable(rec,32);
    differential_wheels_enable_encoders(64);
    robot_step(64);
    while (1) {
		// Wait for data
        while (receiver_get_queue_length(rec) == 0) {
            robot_step(64);
        }

        rbuffer = (double *)wb_receiver_get_data(rec);

		// Check for pre-programmed avoidance behavior
        if (rbuffer[DATASIZE] == -1.0) {
            fitfunc(gwaypoints,100);
            // Otherwise, run provided controller
        } else {
            fit = fitfunc(rbuffer,rbuffer[DATASIZE]);
            buffer[0] = fit;
            wb_emitter_send(emitter,(void *)buffer,sizeof(double));
        }
        wb_receiver_next_packet(rec);
    }
        
    return 0;
}

void compute_wheel_speeds(double x, double y, double theta, double wp_x, double wp_y, int* lspeed, int* rspeed) {
	double goalangle, alpha, distance,speedfactor;
	
	distance = sqrt((wp_x-x)*(wp_x-x)+(wp_y-y)*(wp_y-y));

	goalangle = atan2(wp_y-y,wp_x-x);
	alpha = goalangle - theta;
	if (loc_state == ALIGN)
	{
		if (fabs(alpha) < 0.01) {
			*lspeed = 0;
			*rspeed = 0;
			loc_state = TRANS;
		}
		else
		{
			if (goalangle > theta) {
				if (fabs(alpha) < 3.14159) {
					*lspeed = -25;
					*rspeed = 25;
				}
				else {
					*lspeed = 25;
					*rspeed = -25;
				}
			}
			else {
				if (fabs(alpha) > 3.14159) {
					*lspeed = -25;
					*rspeed = 25;
				}
				else {
					*lspeed = 25;
					*rspeed = -25;
				}
			}
		}
	}
	else {
		while (alpha > M_PI) {
			alpha -= 2 * M_PI;
		}
		while (alpha < -M_PI) {
			alpha += 2 * M_PI;
		}

		// Calculate the speed factor
		speedfactor = (distance + 0.05) * 10. * MAX_SPEED;
		if (speedfactor > MAX_SPEED) speedfactor = MAX_SPEED;

		// Calculate the theoretical speed

		*lspeed = (int)(speedfactor * (M_PI - 2 * fabs(alpha) - alpha) / M_PI + 0.5);
		*rspeed = (int)(speedfactor * (M_PI - 2 * fabs(alpha) + alpha) / M_PI + 0.5);

		// Close to termination condition: just stop
		if (distance < 0.01) {
			*lspeed = 0;
			*rspeed = 0;
		}
	}
}

void odometry(int ds_l, int ds_r, double old_x, double old_y, double old_theta, double* x, double* y, double* theta, double* distance) {
	double ds_t, dtheta;

	ds_t = ((((double)(ds_l + ds_r))/2.0)*WHEEL_RADIUS)/ENCODER_RES;
	dtheta = ((((double)(ds_r - ds_l))/AXLE_LENGTH)*WHEEL_RADIUS)/ENCODER_RES;
    
	*x = old_x + ds_t*cos(old_theta + dtheta/2.0);
	*y = old_y + ds_t*sin(old_theta + dtheta/2.0);
	*theta = old_theta + dtheta;
 
	if (*theta >=  3.14159) 
	{
		*theta = *theta - 2*3.14159;
	}
	else if (*theta < -3.14159) *theta = *theta + 2*3.14159;

	*distance = ds_t;
}

// Find the fitness for obstacle avoidance of the passed controller
double fitfunc(double waypoints[DATASIZE],int its) {
	double fitness = 0.0;             // Fitness of waypoint set
  int j,i;
	int left_encoder, right_encoder, old_left_encoder = 0, old_right_encoder = 0;
	double theta = 0.0, old_theta = 0.0;
	double x = 0.0, y = 0.0;
	int k = 0;
	int ds_l, ds_r, lspeed, rspeed;
	double fit_ds, fit_speed, fit_totalds, fitness_distance, fitness_stuck, fitness_goal, fitness_waypoints;
  
  fit_speed = 0.0;
  fit_ds = 0.0;
  fit_totalds = 0.0;
  
  int count_stuck = 0;
  int count_waypoints = 0;
  int count_waypoints_actual = 0;
  fitness_goal = 0.333;
  
  double waypoints_reached[DATASIZE];
  
  for (i=0;i<DATASIZE;i++) {
    waypoints_reached[i] = 0.0;
  }
  

	wb_differential_wheels_set_encoders(0,0);

	// Evaluate fitness repeatedly
	for (j=0;j<its;j++) {
		// if close enough to current waypoint select next waypoint
		if ((x - waypoints[k])*(x - waypoints[k])+(y - waypoints[k+1])*(y - waypoints[k+1]) < 0.0001) {
			//printf("WAYPOINT REACHED (%.2f,%.2f) YESSS \n",waypoints[k], waypoints[k+1]);
			count_waypoints += 1;
      count_waypoints_actual += 1;
      waypoints_reached[k] = waypoints[k];
      waypoints_reached[k+1] = waypoints[k+1];
      k += 2;
			loc_state = ALIGN;
			if (k == DATASIZE) {
				k = 0;
			}
    }
    

		// compute wheel speeds      
		compute_wheel_speeds(x,y,theta,waypoints[k],waypoints[k+1],&lspeed,&rspeed);
	  	wb_differential_wheels_set_speed(lspeed,rspeed);
		robot_step(128); // run one step

		// compute current position
		left_encoder = wb_differential_wheels_get_left_encoder();
    		right_encoder = wb_differential_wheels_get_right_encoder();

		ds_l = left_encoder - old_left_encoder;
		ds_r = right_encoder - old_right_encoder;
		odometry(ds_l, ds_r, x, y, old_theta, &x, &y, &theta, &fit_ds);
    
    old_left_encoder = left_encoder;
		old_right_encoder = right_encoder;
		old_theta = theta;
    
    fit_speed += (fabs(lspeed) + fabs(rspeed))/(2.0*MAX_SPEED);
    fit_totalds += fit_ds;
    
    if(fit_ds < 0.0001) {
      count_stuck += 1;
    }

		if (y > 1.2) { // when this coordinate is reached the robot is considered escaped and the evaluation stops
			printf("\nROBOT ESCAPED!! :)\n");
      for (i=0;i<DATASIZE;i++) {
        printf("%f ",waypoints_reached[i]);
      }
      printf("\n");
      count_waypoints = 5; //this acts as a bonus for reaching the goal
			fitness_goal = 1.0;
      break;
		}
	}
	
  fit_speed /= its;
  
  fitness_distance = 1/(1+exp(fit_totalds-4));
  fitness_stuck = (1 - (double)count_stuck/its);
  fitness_goal = fitness_goal;
  fitness_waypoints = pow((((double)count_waypoints/10)+0.5),2); //1.2 are assumed to be the length of sides of the world
  
  fitness = pow(fitness_distance,0.75)*pow(fitness_stuck,1.5)*fitness_goal*fitness_waypoints;
  
  /*
  if (fitness > fitness_best) {
    fitness_best = fitness;
    printf("\nFITNESS: %f\nTotal Distance: %.2f\nStuck Ratio: %.2f\n\n", fitness, fit_totalds, fitness_stuck);
  }
  */
  
	printf("FITNESS: %f   waypoints: %d   ditance: %.2f   stuck: %.2f\n", fitness, count_waypoints_actual, fit_totalds, fitness_stuck);
	return fitness;
}

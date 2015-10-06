/*****************************************************************************/
/* File:         follower2.cc                                                */
/* Version:      2.0                                                         */
/* Date:         02-Nov-05                                                   */
/* Description:  Formation movement with E-Pucks                             */
/*                                                                           */
/* Author: 	 22-Oct-04 by nikolaus.correll@epfl.ch                       */
/*****************************************************************************/

#include <stdio.h>
#include <math.h>

#include <webots/robot.h>

#include <webots/differential_wheels.h>
#include <webots/distance_sensor.h>
#include <webots/emitter.h>
#include <webots/receiver.h>

#define NB_SENSORS           8

/* Formation flocking parameters */
#define D                    0.20      // Distance between robots
#define AXLE_LENGTH          0.052     // Distance between wheels of robot
#define SPEED_UNIT_RADS      0.0628    // Conversion factor between speed unit to radian per second
#define WHEEL_RADIUS         0.0205    // Wheel radius in meters
#define DELTA_T              0.064     // Time step in seconds

int Interconn[16] = {-5,-15,-20,6,4,6,3,5,4,4,6,-18,-15,-5,5,3};	
WbDeviceTag ds[NB_SENSORS];           // Handle for the infrared distance sensors
WbDeviceTag receiver;                 // Handle for the receiver node
WbDeviceTag receiver_rb;              // Handle for the receiver of range and bearing information

int robot_id;                       // Unique robot ID

float goal_range   = 0.0;          // goal destination of robot - range component
float goal_bearing = 0.0;          // goal destination of robot - bearing component

float leader_range = 0.0;          // relative range to leader
float leader_bearing = 0.0;        // relative bearing to leader
float leader_orientation = 0.0;    // relative orientation of leader

int iterations;

static void reset(void) {
  wb_robot_init();

  receiver    = wb_robot_get_device("receiver");
  receiver_rb = wb_robot_get_device("receiver_rb");

  int i;

  char s[4]="ds0";
  for(i=0; i<NB_SENSORS;i++) {
    ds[i]=wb_robot_get_device(s);      // the device name is specified in the world file
    s[2]++;                         // increases the device number
  }
  char* robot_name; robot_name=(char*) wb_robot_get_name(); 
  
  sscanf(robot_name,"epuck%d",&robot_id);    // read robot id from the robot's name
  printf("Reset: robot %d\n",robot_id);
}

// Updates leader's position (in range and bearing) with respect to robot's position,
// after robot's self-locomotion.
void update_self_motion(int msl, int msr) {
  // Easier to transform everything in x,y,theta coordinates
  // Coordinates of leader relative to robot
  float x = leader_range * cosf(leader_bearing);
  float y = leader_range * sinf(leader_bearing);
  float theta = leader_orientation;

  // Account for self-locomotion
  // Compute deltas of the robot
  float u = (float)(msl + msr)/2.0 * SPEED_UNIT_RADS * WHEEL_RADIUS;
  float dx = u * DELTA_T;
  float dy = 0.0;
  float dtheta = (float)(msr - msl)/AXLE_LENGTH * SPEED_UNIT_RADS * WHEEL_RADIUS * DELTA_T;
  // Update leader x, y, theta (as if it did not move)
  x -= dx;
  float t = x * cosf(-dtheta) - y * sinf(-dtheta);
  y = x * sinf(-dtheta) + y * cosf(-dtheta);
  x = t;
  theta -= dtheta;
  
  // Transform back in range and bearing
  leader_range = sqrtf(x*x + y*y);
  leader_bearing = atan2f(y, x);
  leader_orientation = theta;
  // Keep orientation within -pi, pi
  if (leader_orientation > M_PI) leader_orientation -= 2.0*M_PI;
  if (leader_orientation < -M_PI) leader_orientation += 2.0*M_PI;
}


void update_leader_motion(int nsl, int nsr) {
  
  /* PUT YOUR CODE HERE */
  float x = leader_range * cosf(leader_bearing);
  float y = leader_range * sinf(leader_bearing);
  float theta = leader_orientation;
  
  // Account for the leader motion
  // Compute delta x, y, theta
  float u = (float)(nsl + nsr)/2.0 * SPEED_UNIT_RADS * WHEEL_RADIUS;
  float dx = u * cosf(theta) * DELTA_T;
  float dy = u * sinf(theta) * DELTA_T;
  float dtheta = (float)(nsr - nsl)/AXLE_LENGTH * SPEED_UNIT_RADS * WHEEL_RADIUS * DELTA_T;
  // Compute new x, y, theta
  x += dx;
  y += dy;
  theta += dtheta;

  // Transform back in range and bearing
  
  leader_range = sqrtf(x*x + y*y);
  leader_bearing = atan2f(y, x);
  leader_orientation = theta;
  
  // Keep orientation within -pi, pi
  if (leader_orientation > M_PI) leader_orientation -= 2.0*M_PI;
  if (leader_orientation < -M_PI) leader_orientation += 2.0*M_PI;
}

void update_leader_measurement(float new_leader_range, float new_leader_bearing, float new_leader_orientation) {
  /* PUT YOUR CODE HERE */
  
  // this part gets executed after specific intervals of time
  // point is that the followers get exact measurements to update their possibly faulty predictions of range and bearing after fixed intervals
  if(iterations%50==0) {
    leader_range = new_leader_range;
    leader_bearing = new_leader_bearing;
    leader_orientation = new_leader_orientation;
  }
}

void compute_wheel_speeds(int nsl, int nsr, int *msl, int *msr) {
  /* PUT YOUR CODE HERE */
  
  // Define constants
  float Ku = 0.2;
  float Kw = 10.0;
  float Kb = 1.0;

  // Compute the range and bearing to the wanted position
  float x = leader_range * cosf(leader_bearing);
  float y = leader_range * sinf(leader_bearing);
  float theta = leader_orientation;
  x += goal_range * cosf(- M_PI + goal_bearing + theta);
  y += goal_range * sinf(- M_PI + goal_bearing + theta);
  float range = sqrtf(x*x + y*y); // This is the wanted position (range)
  float bearing = atan2(y, x);    // This is the wanted position (bearing)
  
  // Compute forward control (proportional to the projected forward distance to the leader
  float u = Ku * range * cosf(bearing);
  // Compute rotional control
  float w = Kw * range * sinf(bearing) + Kb * leader_orientation;
  // Of course, we can do a lot better by accounting for the speed of the leader (rather than just the position)
 
  // Convert to wheel speeds!
  *msl = (int)((u - AXLE_LENGTH*w/2.0) / (SPEED_UNIT_RADS * WHEEL_RADIUS));
  *msr = (int)((u + AXLE_LENGTH*w/2.0) / (SPEED_UNIT_RADS * WHEEL_RADIUS));
}

int main(){
  
  int msl,msr;                      // motor speed left and right
  float new_leader_range, new_leader_bearing, new_leader_orientation; // received leader range and bearing
  int distances[NB_SENSORS];        // array keeping the distance sensor readings
  char *inbuffer;                   // buffer for the receiver node
  float *rbbuffer;                  // buffer for the range and bearing
  int nsl, nsr;                     // Neighbor speed, left and right
  int i;
  iterations = 1;
 
  reset();                          // Initialization 
  for(i=0;i<NB_SENSORS;i++)
    wb_distance_sensor_enable(ds[i],64);
  wb_receiver_enable(receiver,64); 
  wb_receiver_enable(receiver_rb,64); 
 
  // Initialize neighbor speeds (just in case)
  nsl=0; nsr=0;

  /* Wait until supervisor sent range and bearing information */
  while (wb_receiver_get_queue_length(receiver_rb) == 0) {
    wb_robot_step(64); // Executing the simulation for 64ms
  }
  rbbuffer = (float*) wb_receiver_get_data(receiver_rb);
  goal_range = sqrt(rbbuffer[0]*rbbuffer[0] + rbbuffer[1]*rbbuffer[1]);
  goal_bearing = -atan2(rbbuffer[0],rbbuffer[1]);
  printf("Goal of robot %d: range = %.2f, bearing = %.2f\n", robot_id, goal_range, goal_bearing);
  leader_range = goal_range;
  leader_bearing = goal_bearing;
  leader_orientation = rbbuffer[2]; // Initial orientation with respect to this robot
  printf("  Initial relative position: range = %.2f, bearing = %.2f, heading = %.2f\n", leader_range, leader_bearing, leader_orientation);
  wb_receiver_next_packet(receiver_rb);

  msl=0; msr=0;  

  for(;;){
    int sensor_nb;
    int bmsl = 0;
    int bmsr = 0;
    for(sensor_nb=0;sensor_nb<NB_SENSORS;sensor_nb++){  // read sensor values and calculate motor speeds
      distances[sensor_nb]=wb_distance_sensor_get_value(ds[sensor_nb]);
      /* Weighted sum of distance sensor values for Braitenburg vehicle */
      bmsr += distances[sensor_nb] * Interconn[sensor_nb];
      bmsl += distances[sensor_nb] * Interconn[sensor_nb + NB_SENSORS];
    }
    bmsl /= 400; bmsr /= 400;        // Normalizing speeds
    

    /* Receiver leader wheel speeds */
    if (wb_receiver_get_queue_length(receiver) > 0) {
      inbuffer = (char*) wb_receiver_get_data(receiver);
      sscanf(inbuffer,"%d#%d",&nsl,&nsr);                  // extract wheelspeed from message
      wb_receiver_next_packet(receiver);
    }

    update_self_motion(msl,msr);
    update_leader_motion(nsl,nsr);

    /* Receiver leader range and bearing */
    if (wb_receiver_get_queue_length(receiver_rb) > 0) {
      rbbuffer = (float*) wb_receiver_get_data(receiver_rb);
      new_leader_range = sqrt(rbbuffer[0]*rbbuffer[0] + rbbuffer[1]*rbbuffer[1]);
      new_leader_bearing = -atan2(rbbuffer[0],rbbuffer[1]);
      new_leader_orientation = rbbuffer[2];

      update_leader_measurement(new_leader_range, new_leader_bearing, new_leader_orientation);

      wb_receiver_next_packet(receiver_rb);
      
      iterations += 1;
    }

    compute_wheel_speeds(nsl, nsr, &msl, &msr);

    msl += bmsl;
    msr += bmsr;
    wb_differential_wheels_set_speed(msl,msr);
    wb_robot_step(64);               // Executing the simulation for 64ms
  }
}  
  

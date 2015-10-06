
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <webots/robot.h>
#include <webots/differential_wheels.h>
#include <webots/supervisor.h>
#include <webots/receiver.h>

#define TIME_STEP 64
#define COMMUNICATION_CHANNEL 1

// RUN VARIABLES

#define NUM_ROBOTS        1
#define COMM_RADIUS       0.2
#define MIN_CONNECTIONS   1
#define EXP_TIME          1000

// Auxiliary

#define ARENASIDE         200
#define BEACON_TIMER      60






int main(int argc, char *argv[]) {

  wb_robot_init();
  
  WbNodeRef robot_node = wb_supervisor_node_get_from_def("rob0");
  WbFieldRef trans_field = wb_supervisor_node_get_field(robot_node, "translation");
  
  double count = 0;
  
  while(1) {
    
    char time[10];
    
    sprintf(time,"%f sec",count);
  
    wb_supervisor_set_label(0,time,0,0,0.1,0xff0000,0);

    const double *trans = wb_supervisor_field_get_sf_vec3f(trans_field);
    //printf("MY_ROBOT is at position: %g %g %g\n", trans[0], trans[1], trans[2]);
  
    if (x==1 && trans[0]<-1.7 && trans[2]<-0.65) {
      wb_supervisor_export_image ("/home/afroze/Desktop/photo.jpg",50);
      printf("\n\n\n\nPHOTOOOOOO\n\n\n\n\n");
      return 0;
    }
  
    wb_robot_step(TIME_STEP);  
    count+=0.064;
    
  }
  
  return 0;
}


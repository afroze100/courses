#include "i12supervisor.h"

extern robot robots[MAX_ROBOTS];

/* Threshold Calculations */
int type[MAX_ROBOTS];

int check_threshold(int r){
  
  //printf("%f\n",RAND);
  if(RAND < sigmoid(robots[r].threshold[0], robots[r].threshold[1]))
  {
    if(RAND < sigmoid(robots[r].threshold[0], robots[r].stimulus[0])) 
      return 0;
    else 
      return -1;
  }
  else 
  {
    if(RAND < sigmoid(robots[r].threshold[1], robots[r].stimulus[1])) 
      return 1;
    else 
      return -1;
  }
  
}

/* Adaptation */

void adapt_threshold_on_task(int i, int r){
  
  if (robots[r].threshold[i]>20)
    robots[r].threshold[i] = robots[r].threshold[i]-10;
  else if (robots[r].threshold[i]>10)
    robots[r].threshold[i] = robots[r].threshold[i]-5;
  else if (robots[r].threshold[i]>2)
    robots[r].threshold[i] = robots[r].threshold[i]-1;
  else if (robots[r].threshold[i]>0.5)
    robots[r].threshold[i] = robots[r].threshold[i]-0.5;
  else if (robots[r].threshold[i]>0.1)
    robots[r].threshold[i] = robots[r].threshold[i]-0.1;
  else if (robots[r].threshold[i]>0)
    robots[r].threshold[i] = robots[r].threshold[i]-0.01;
  //DBG(("Threshold on task %d (robot %d): %f\n", i, r, robots[r].threshold));
}


void adapt_threshold_on_no_task(int i, int r){
  
  if (robots[r].threshold[i]<0.5)
    robots[r].threshold[i] = robots[r].threshold[i]+0.001;
  else if (robots[r].threshold[i]<2)
    robots[r].threshold[i] = robots[r].threshold[i]+0.002;
  else if (robots[r].threshold[i]<10)
    robots[r].threshold[i] = robots[r].threshold[i]+0.004;
  else if (robots[r].threshold[i]<20)
    robots[r].threshold[i] = robots[r].threshold[i]+0.008;
  else
    robots[r].threshold[i] = robots[r].threshold[i]+0.016;
  //DBG(("Threshold on no task %d (robot %d): %f\n", i, r, robots[r].threshold));
}

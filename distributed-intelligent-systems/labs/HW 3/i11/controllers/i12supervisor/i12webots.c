#ifdef WEBOTS
#include "i12supervisor.h"
#include <unistd.h>


/* Webots Defines */

#define WEBOTS_CORNER_X 3
#define WEBOTS_CORNER_Y 3


/* External Variables */

extern robot robots[];
extern struct eventlist *events[2];
extern int n_robots, n_events[2];
extern int total_events_handled;
extern float (*evaluate_solution)();
extern unsigned long long int activity;


/* Webots Global Variables */

static WbNodeRef robot_node[MAX_ROBOTS];
static WbNodeRef event_node[2][MAX_EVENTS];
static WbDeviceTag emitter;
static unsigned long long int timestamp;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* Webots Code */

//Links up all the nodes we are interested in.
//Gets called by webots at robot_live(reset)
static void wbgui_reset( void){
  int i, j;
  char s[50];
  char *robotprefix = "r";
  char *eventprefix[2] = {"e", "g"};

  
  //DBG(("wbgui_reset()\n")); 

  timestamp = 0; 

  //get the emitter
  emitter = wb_robot_get_device("emitter");

  //get the robots
  for ( i=0; i<n_robots; i++ ){
    sprintf(s, "%s%d", robotprefix, i);
    robot_node[i] = wb_supervisor_node_get_from_def(s);
    //DBG(("robot %d linked\n", i));
  }

  //get the events
  for (j=0; j<2; ++j){
    for (i=0; i<MAX_EVENTS; i++){
      sprintf(s, "%s%d", eventprefix[j], i);
      event_node[j][i] = wb_supervisor_node_get_from_def(s);
      //DBG(("event %d %d linked\n", j, i));
    }
  }
}

//The function that calls the guts of our program
//and lets webots know where to put things.
//gets called by robot_run(wbgui_run)
static int wbgui_run(){
  int i, eindex=0;
  run_one_iteration();
  if (total_events_handled == TOTAL_EVENTS_TO_HANDLE){
    printf("----- %d Events Handled in %.3f seconds, %.2f percent active. -----\n\n", total_events_handled, timestamp/1000.0, (double)activity/n_robots/timestamp*100);
    fflush(stdout);
    sleep(1);
    wb_supervisor_simulation_revert(); // start over
  }
  
  double pos[MAX_ROBOTS][3];
  double epos[MAX_EVENTS][3];
  event *e;

  //Place the robots
  for (i=0; i<n_robots; i++ ){
    if (timestamp > 0)
    {
      if (robot_node[i]==0)
        printf(" robot %d missing...\n", i);
      else{
        pos[i][0] = ROBOT2WEBOT(robots[i].x);
        pos[i][1] = .01;
        pos[i][2] = ROBOT2WEBOT(robots[i].y);
        //supervisor_field_set(robot_node[i], SUPERVISOR_FIELD_TRANSLATION, &(pos[i]));
        wb_supervisor_field_set_sf_vec3f(wb_supervisor_node_get_field(robot_node[i],"translation"), pos[i]);

      }
    }
  }

  //Place the events. If there are fewer than n_events in the
  //events list (perhaps because some are in the auction list), put
  //those events in the corner.
  for (i=0; i<2; ++i)
  {
    for (e=FIRST_ITEM(events[i]); e!= NULL; e=NEXT_ITEM(e)){
      epos[eindex][0] = ROBOT2WEBOT(e->x);
      epos[eindex][1] = .01;
      epos[eindex][2] = ROBOT2WEBOT(e->y);
      //supervisor_field_set(event_node[eindex],  SUPERVISOR_FIELD_TRANSLATION, &(epos[eindex]));
			   
      wb_supervisor_field_set_sf_vec3f(wb_supervisor_node_get_field(event_node[i][eindex],"translation"), epos[eindex]);
      eindex++;
    }
  }

  //Advance the world WBGUI_STEP ms before calling wbgui_run again
  timestamp += WBGUI_STEP;
  return WBGUI_STEP;
}

void run_webots(){

  wbgui_reset();
  
  wb_robot_step(64);
   while (1) {
    	wbgui_run();
    	wb_robot_step(64);
   }
}


#endif /* WEBOTS */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

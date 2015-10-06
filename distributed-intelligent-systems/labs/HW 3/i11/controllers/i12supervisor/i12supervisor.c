/* Problem Description */

// (change these values according to the question you are working on)
#define ADAPTIVE        1      // 0=fixed, 1=adaptive thresholds

#define THRESHOLD       5       // value of homogeneous threshold
#define ARRIVAL_RATE_0  0.02   // probability of a new event of type 0
#define ARRIVAL_RATE_1  0.02   // probability of a new event of type 1
#define SENSOR_RANGE    65      // range of local perception
#define EXPONENT        1      // steepness of threshold cutoff


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "i12supervisor.h"
#include <time.h>


/* Global Variables */

int n_robots=5;
int n_initialevents[2]={3,3};
int n_events[2]={0,0};
int xdim=100, ydim=100;

robot robots[MAX_ROBOTS];           // an array of robots
struct eventlist events_S[2], *events[2]; // a linked list of open events
unsigned long long int activity;    // total time robots have spent moving

int total_events_handled = 0;       // number of events already handled by team
int current_event_id=0;             // counter (for generating unique id nums


/* Local Function Headers */

event *create_new_event();

float sigmoid(float inflection, float input){
  return (pow(input,EXPONENT))/(pow(input,EXPONENT)+pow(inflection,EXPONENT));
}

/* Robot Behavior */

int update_robot(int r){
  // check distance to all events, and handle if close enough
  robots[r].handling_type = check_threshold(r);
  if (robots[r].handling_type >= 0)
  {
  activity += WBGUI_STEP;

  robots[r].theta += (RAND-0.5)/2;
  robots[r].x += ROBOT_STEP_SIZE*cos(robots[r].theta);
  robots[r].y += ROBOT_STEP_SIZE*sin(robots[r].theta);
  if (robots[r].x > xdim) { robots[r].x = xdim; robots[r].theta += M_PI/2; }
  if (robots[r].x < 0)    { robots[r].x = 0;    robots[r].theta += M_PI/2; }
  if (robots[r].y > ydim) { robots[r].y = ydim; robots[r].theta += M_PI/2; }
  if (robots[r].y < 0)    { robots[r].y = 0;    robots[r].theta += M_PI/2; }
  robots[r].distance_traveled += ROBOT_STEP_SIZE;
                    
  event *ev = FIRST_ITEM(events[robots[r].handling_type]);
  while (ev)
  {
    if (DIST(&robots[r], ev) < OK_DIST_TO_GOAL){
      // handled an event
      //DBG(("Robot %d handled event %d\n", r, ev->id));
      TAILQ_REMOVE(events[robots[r].handling_type], ev, list);
      free(ev);
      n_events[robots[r].handling_type]--;
      if(ADAPTIVE){
         adapt_threshold_on_task(robots[r].handling_type, r);
         adapt_threshold_on_no_task(!robots[r].handling_type, r);
      }
      return 1; 
    }
    ev = NEXT_ITEM(ev);
  }
  }

  // no event handled
  if(ADAPTIVE){
    adapt_threshold_on_no_task(0, r);
    adapt_threshold_on_no_task(1, r);
  }
  return 0;
}

int nearby_events(int i, int r){
  // count events within SENSOR_RANGE
  int local_events = 0; 
  event *ev = FIRST_ITEM(events[i]);
  while (ev)
  {
    if (DIST(&robots[r], ev) < SENSOR_RANGE) local_events++;
    ev = NEXT_ITEM(ev);
  }
  return local_events;
}


/* Simulation Execution (body of main program loop) */

void run_one_iteration(){
  int r, n_finished_events=0;

  //1) all robots move (if desired)
  for (r=0; r<n_robots; r++)
  {
    n_finished_events += update_robot(r);
    printf("%d => %.3f %.3f\n, ",r,robots[r].threshold[0], robots[r].threshold[1]);
  }
  
  //2) add new events randomly
  if (RAND < ARRIVAL_RATE_0 && n_events[0] + n_events[1] < MAX_EVENTS)
    create_new_event(0);

  if (RAND < ARRIVAL_RATE_1 && n_events[0] + n_events[1] < MAX_EVENTS)
    create_new_event(1);
 
  //3) estimate stimulus
  for (r=0; r<n_robots; r++)
  {
    robots[r].stimulus[0] = nearby_events(0, r);
    robots[r].stimulus[1] = nearby_events(1, r);
  }
  
  //4) keep track of how the team is doing
  total_events_handled += n_finished_events;

  //5) print status of adaptation
  if (ADAPTIVE)
    for (r=0; r<n_robots; r++)
    {
      //DBG(("  robots[%d].threshold = %f\n",r,robots[r].threshold));
    }
      
}


/* Initialization */

void initialize_robots(){
  int r;
  // Give robots a random position and initialize their task list to empty
  for(r=0; r<n_robots; r++){
    RANDOM_POSITION(&robots[r]);
    robots[r].tasks = &(robots[r].tasks_S);
    TAILQ_INIT(robots[r].tasks);
    robots[r].tour_length = 0;
    robots[r].distance_traveled = 0;
    //threshold specific values
    robots[r].theta = RAND_THETA; 
    robots[r].stimulus[0] = robots[r].stimulus[1] = 0; 
    robots[r].handling_type = -1; 
    robots[r].threshold[0] = THRESHOLD;
    robots[r].threshold[1] = THRESHOLD;
  }
}

/* Create a new event with a random position and insert it into the list */
event *create_new_event(int i){
  event *ev = (event *) malloc(sizeof(event));
  RANDOM_POSITION(ev);
  TAILQ_INSERT_TAIL(events[i], ev, list);
  ev->id = current_event_id++;
  n_events[i]++;
  //DBG(("New event[%d] %d created at %.2f %.2f  (%d unhandled)\n", i, ev->id, ev->x, ev->y, n_events[i]));
  return ev;
}


/* Initialize the list of events and create new events*/
void initialize_events(){
  int e, i;
  for (i = 0; i < 2; ++i)
  {
    events[i] = &events_S[i];
    TAILQ_INIT(events[i]);
    for (e=0; e<n_initialevents[i]; e++)
      create_new_event(i);
  }
}

void initialize(){
  srand(time(NULL));
  initialize_robots();
  initialize_events();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* Main */

int main(int argc, char **argv){
  #ifdef WEBOTS
  wb_robot_init();
  #endif
  
  initialize();

#ifdef WEBOTS
  run_webots();
#else

  // COMMAND LINE OPERATION NOT YET PROPERLY SUPPORTED.

#endif

   return 0;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

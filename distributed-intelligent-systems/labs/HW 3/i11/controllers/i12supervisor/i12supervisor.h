#ifndef I12SUPERVISOR_H
#define I12SUPERVISOR_H


/* System includes */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/queue.h>
#include <getopt.h>

#ifdef WEBOTS
#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/emitter.h>
#endif


/* Defines */

#define MAX_ROBOTS              50
#define MAX_EVENTS              50
#define ROBOT_STEP_SIZE         1
#define OK_DIST_TO_GOAL         3
#define TOTAL_EVENTS_TO_HANDLE  100

#ifdef WEBOTS
#define WBGUI_STEP              64
#define ROBOT_Z                 2
#define MSG(x) robot_console_printf x ;
#else
#define MSG(x) printf x ; fflush(stdout);
#endif


/* Macros */

#ifdef QUIET
#define DBG(x)
#else
#define DBG(x) printf x ; fflush(stdout)
#endif

#define RAND ((float) rand()/RAND_MAX)
#define RAND_THETA ((float) rand()/RAND_MAX)*M_PI*2

#define RANDOM_POSITION(a) do{			\
  (a)->x = ((float) rand()/RAND_MAX)*xdim;	\
  (a)->y = ((float) rand()/RAND_MAX)*ydim;	\
}while(0)

#define FIRST_ITEM(a) (a)->tqh_first
#define NEXT_ITEM(a) (a)->list.tqe_next
#define THETA(a, b) atan2((b)->y - (a)->y, (b)->x - (a)->x)
#define DIST(a, b) sqrt(((a)->x - (b)->x)*((a)->x - (b)->x) + \
  ((a)->y - (b)->y)*((a)->y - (b)->y))
#define IN_RANGE(a, amin, amax) ((a >= amin) && (a <= amax))
#define SQ(a) (a * a)
#define ROBOT2WEBOT(x) x/100 - .5


/* Structures */

struct event_struct{
  int id;
  float x;
  float y;
  TAILQ_ENTRY(event_struct) list;
};
typedef struct event_struct event;
TAILQ_HEAD(eventlist, event_struct);

struct task_struct{
  event *ev;
  TAILQ_ENTRY(task_struct) list;
};
typedef struct task_struct task;
TAILQ_HEAD(tasklist, task_struct);

typedef struct{
  float x;    //current (x,y) location of the robot
  float y;
  struct tasklist tasks_S;
  struct tasklist *tasks;
  task *task_at_auction;
  float tour_length;
  float distance_traveled;
  // threshold specific values
  float threshold[2];
  float stimulus[2];
  float theta; // current heading angle
  int handling_type;
} robot;

typedef struct{
  int from;
  event *ev;
  float amount;
} bid;


/* Prototypes */

void step();
void run_one_iteration();
void adapt_threshold_on_task(int i, int r);
void adapt_threshold_on_no_task(int i, int r);
int check_threshold(int r);

#ifdef WEBOTS
void run_webots();
#endif

float sigmoid(float inflection, float input);

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

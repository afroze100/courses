//
//  supervisor_crown
//
//  $Author$
//  $Date: 2007-12-03 17:18:40 +0100 (Mon, 03 Dec 2007) $
//  $Revision$
//  $HeadURL: http://grmapc10.epfl.ch/svn/students/Lab9/webots/controllers/supervisor_crown/supervisor_crown.cc $
//


#include <cassert>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <iostream>

#include <device/emitter.h>
#include <device/receiver.h>
#include <device/supervisor.h>
#include <device/robot.h>


#define ROBOTS 10
#define STEP_SIZE 64

using namespace std;

static NodeRef rob[ROBOTS];         // References to robots
static NodeRef emi[ROBOTS];         // References to robot emitters 
DeviceTag      rec[ROBOTS];         // Supervisor receivers

char status[ROBOTS]; 
bool printed;
int print_check = 0;


/* Reset the supervisor */
void reset(void) {
  const char rob_prefix[] = "epuck_";   // robot naming convention
  const char rec_prefix[] = "rec";      // supervisor receives from source
  const char emi_prefix[] = "emi";      // supervisor emits to robot mics

  ostringstream rob_name;               //robot
  ostringstream emi_name;

  ostringstream rec_name;               //supervisor

  int i;

  printed = false; 

  for (i=0;i<ROBOTS;i++)
  {
    /* Get and save a reference to the robot. */
    rob_name.str("");
    rob_name << rob_prefix << i+1; // << "_0"; 
    rob[i] = supervisor_node_get_from_def(rob_name.str().c_str());

    /* Get robot emitters */
    emi_name.str("");
    emi_name << emi_prefix << i+1;
    emi[i] = supervisor_node_get_from_def(emi_name.str().c_str());

    /* Get supervisor receivers */
    rec_name.str("");
    rec_name << rec_prefix << i+1;
    rec[i] = robot_get_device(rec_name.str().c_str());
    if (rec[i]==0) robot_console_printf("missing receiver %d\n",i);
    receiver_enable(rec[i],32);
  }
}

bool converged(void)
{
  int i;
  char dir = status[0];

  /*
  cout << "status = ";
  for (i=0; i<ROBOTS; i++)
    cout << (int)(status[i]) << ' ';
  cout << endl;
  */

  for (i=1;i<ROBOTS;i++)
    if (status[i] != dir) return false;

  return true;
}

/* Repeatedly find the sound received at each microphone, and send to them */
static int run(int ms) {

  static unsigned long long int clock = 0;

  char *buf;            // Data from robots
  int i;                // FOR-loop counter
  
  int left_robots = 0;
  int right_robots = 0;

  /* Get data */
  for (i=0;i<ROBOTS;i++) {
    /* Check if we're receiving data */
    if(receiver_get_queue_length(rec[i]) > 0) 
    {
      assert(receiver_get_queue_length(rec[i])>0);
      assert(receiver_get_data_size(rec[i])==1);
      buf = (char*)receiver_get_data(rec[i]);
      status[i] = *buf; 
      receiver_next_packet(rec[i]);
    }
  }

  if (!printed && converged())
  {
      printf("===== CONVERGED IN %.3f SECONDS =====\n\n",clock/1000.0);
      fflush(stdout);
      printed = true;
      //supervisor_simulation_quit();
  }

  for (i=0;i<ROBOTS;i++)
  {
    if (status[i] == 1)
      left_robots += 1;
    if (status[i] == 2)
      right_robots += 1;
  }
  
  //printf("right = %d, left = %d\n",right_robots,left_robots);
  
  if (print_check == 0)
  {
    if(left_robots == 7 && right_robots ==3)
    {
      printf("===== CONVERGED IN %.3f SECONDS =====\n\n",clock/1000.0);
      print_check = 1;
    }
    else if(left_robots == 3 && right_robots ==7)
    {
      printf("===== CONVERGED IN %.3f SECONDS =====\n\n",clock/1000.0);
      print_check = 1;
    }
  }

  clock+=STEP_SIZE;
  return STEP_SIZE;
}


// main loop
int main(void) 
{
  // initialization
  robot_live(reset);
  robot_step(2*STEP_SIZE);

  // start the controller
  robot_console_printf("Starting main loop...\n");
  robot_run(run);
  return 0;
}


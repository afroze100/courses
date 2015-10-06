/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * file:        epuck_crown.c
 * author:      Christopher M. Cianci
 * description: Collective decision / wall-following with e-Pucks and radios.
 *
 * $Revision$
 * $Date$
 * $Author$
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <unistd.h>

#include <sstream>
using namespace std;

#include <device/robot.h>
#include <device/differential_wheels.h>
#include <device/emitter.h>
#include <device/receiver.h>
#include <device/distance_sensor.h>
#include <device/radio.h>

#define DEBUG 1

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Collective decision parameters */

#define OPCHANGE_PROB       0.5     // Probability of changing opinion
#define RIGHT_WALL_PROB     0.5     // Probability of initially selecting RIGHT
#define COM_POWER           0.001   // Transmission power in watts (almost)
#define DECISION_INTERVAL   1       // Frequency of communication in seconds
enum follow_state {
    RANDOM  = 0,                    // Initial state aliases
    LEFT    = 1,
    RIGHT   = 2,
};



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* e-Puck parameters */

#define NB_SENSORS           8
#define BIAS_SPEED           400

int Interconn[16] = {8,-4,-6,3,5,4,4,6,-15,-15,-5,5,3,-5,-15,-20};
//
//{5,-4,-6,3,5,4,4,-20,-20,-15,-5,5,3,-5,-15,-15};

DeviceTag em;
static DeviceTag ds[NB_SENSORS];    // Handle for the infrared distance sensors
static DeviceTag radio;             // Radio
int robot_id;                       // Unique robot ID
char state;                         // Wall following state
int table [9][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}}; //{id,state,timeout}
int sorted_table [10];
int index_table = 0;

int lmsg, rmsg;                     // Communication variables
double right_rob,left_rob;
int maj_min = 0; // 0 = no info, 1 = majority, -1 = minority
int stable_state = 0;
int repeat = 0;

static char display[256]="";



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* helper functions */

// Generate random number in [0,1]
double rnd(void) {
  return ((double)rand())/((double)RAND_MAX);
}


static void radio_callback(RadioEvent event) 
{
    /*
    snprintf(display,256,
    //robot_console_printf(
        "received \"%s\" (%d bytes) from %s "
        "with RSSI=%g on radio %d",
        radio_event_get_data(event),
        radio_event_get_data_size(event),
        radio_event_get_emitter(event),
        radio_event_get_rssi(event),
        radio_event_get_radio(event));
    */
    
    int exists = 0;
    int index;
    
    right_rob = 0;
    left_rob = 0;
    maj_min = 0;
    
    char id_1[3];
    id_1[0] = radio_event_get_data(event)[0];
    id_1[1] = radio_event_get_data(event)[1];
    id_1[2] = radio_event_get_data(event)[2];
    
    int id = id_1[0]*100 + id_1[1]*10 + id_1[2];
    int direction = radio_event_get_data(event)[3];
    
    
    for(int i=0;i<9;i++)
    {
      if(table[i][0] == id)
      {
        exists = 1;
        index = i;
      }
    }
    
    if(exists==0) // first time encounter
      {
      table[index_table][0] = id;
      table[index_table][1] = direction;
      table[index_table][2] = 500;
      index_table += 1;
      }
    else
      {
      table[index][1] = direction;
      table[index][2] = 500;
      }
      
    for(int i=0;i<9;i++)
    {
      if(table[i][1] == RIGHT)
        right_rob += 1;
      else if(table[i][1] == LEFT)
        left_rob += 1;
    }
    
    if(state == RIGHT)
      right_rob += 1;
    else if(state == LEFT)
      left_rob += 1;
    
    if((right_rob+left_rob)>=5)
      {
      if(state == RIGHT && right_rob >= left_rob)
        maj_min = 1;
      else if(state == RIGHT && right_rob < left_rob)
        maj_min = -1;
      else if(state == LEFT && right_rob > left_rob)
        maj_min = -1;
      else if(state == LEFT && right_rob <= left_rob)
        maj_min = 1;
      }
      
    
    if((right_rob + left_rob) == 10)
      stable_state = 1;
    
    //printf("%i => %i\n",id,radio_event_get_data(event)[3]);
    
    /*
    printf("%i => ",robot_id);
    for(int i=0;i<9;i++);
      //printf("[%d,%d,%d] ",table[i][0],table[i][1],table[i][2]);
    printf("  s = %d  ,  r = %d  ,  l = %d\n",state,(int)right_rob,(int)left_rob);
    */
    
    if(direction == RIGHT) rmsg++;
    if(direction == LEFT ) lmsg++;
}


/* Check for change of opinion */
int changeOpinion() 
{
    
    char temp;
    // Check for change from left to right
    /*
    if(maj_min == 0)
    {
      if (state == LEFT && rmsg > lmsg) {
          if (rnd() < OPCHANGE_PROB) {
              printf("robot %d changes to right\n",robot_id);
              temp = RIGHT;
              emitter_send_packet(em,&temp,sizeof(char));
              return RIGHT;
          } 
      }

      // Check for change from right to left 
      if (state == RIGHT && lmsg > rmsg) {
          if (rnd() < OPCHANGE_PROB) {
              printf("robot %d changes to left\n",robot_id);
              temp = LEFT;
              emitter_send_packet(em,&temp,sizeof(char));
              return LEFT;
          }
      }
    }
    */
    
    if(maj_min == 0 && stable_state == 0)
    {
      if (state == LEFT && lmsg > rmsg && double(lmsg)/rmsg > 2.33) {
          if (rnd() < OPCHANGE_PROB) {
              printf("robot %d changes to right\n",robot_id);
              temp = RIGHT;
              emitter_send_packet(em,&temp,sizeof(char));
              return RIGHT;
          } 
      }

      // Check for change from right to left 
      if (state == RIGHT && rmsg > lmsg && double(rmsg)/lmsg > 2.33) {
          if (rnd() < OPCHANGE_PROB) {
              printf("robot %d changes to left\n",robot_id);
              temp = LEFT;
              emitter_send_packet(em,&temp,sizeof(char));
              return LEFT;
          }
      }
      
      if (state == LEFT && rmsg > lmsg && double(lmsg)/rmsg > 0.43) {
          if (rnd() < OPCHANGE_PROB) {
              printf("robot %d changes to right\n",robot_id);
              temp = RIGHT;
              emitter_send_packet(em,&temp,sizeof(char));
              return RIGHT;
          } 
      }

      // Check for change from right to left 
      if (state == RIGHT && lmsg > rmsg && double(rmsg)/lmsg > 0.43) {
          if (rnd() < OPCHANGE_PROB) {
              printf("robot %d changes to left\n",robot_id);
              temp = LEFT;
              emitter_send_packet(em,&temp,sizeof(char));
              return LEFT;
          }
      }
    }
    
    else if(stable_state == 0)
    {
      if(maj_min == 1 && state == LEFT && left_rob/right_rob > 2.33) {
        if (rnd() < OPCHANGE_PROB)
          return RIGHT;
      }
      else if(maj_min == 1 && state == RIGHT && right_rob/left_rob > 2.33){
        if (rnd() < OPCHANGE_PROB)  
          return LEFT;
      }
      else if(maj_min == -1 && state == LEFT && left_rob/right_rob > 0.43){
        if (rnd() < OPCHANGE_PROB)
          return RIGHT;
      }
      else if(maj_min == -1 && state == RIGHT && right_rob/left_rob > 0.43){
        if (rnd() < OPCHANGE_PROB)  
          return LEFT;
      }
    }
    
    
    else if(stable_state == 1 && repeat == 0)
    {
      int x;
      
      for(int i=0;i<9;i++)
      {
        sorted_table[i] = table[i][0];
      }
      sorted_table[9] = robot_id;
      
      int temp;   // holding variable
      int numLength = 10;
      for (int i=0; i< (numLength -1); i++)    // element to be compared
      {
        for(int j = (i+1); j < numLength; j++)   // rest of the elements
        {
          if (sorted_table[i] < sorted_table[j])          // descending order
          {
            temp= sorted_table[i];          // swap
            sorted_table[i] = sorted_table[j];
            sorted_table[j] = temp;
          }
        }
      }
      
      repeat = 1;
      
      printf("%i => [",robot_id);
      for(int i=0;i<10;i++)
        printf("%d  ",sorted_table[i]);
      printf("]\n");
      
      if (robot_id <= sorted_table[7])
        return LEFT;
      else
        return RIGHT;
    }
    
    return state;
}












static void reset ( void ) 
{
    int i; // FOR-loop counter
  
    ostringstream emi_name;

    char s[4]="ps0";
    for(i=0; i<NB_SENSORS;i++) 
    {
        // the device name is specified in the world file
        ds[i]=robot_get_device(s);      
        s[2]++; // increases the device number
        distance_sensor_enable(ds[i],64);
    }

    // read robot id and state from the robot's name
    char* robot_name; robot_name=(char*) robot_get_name(); 
    sscanf(robot_name,"epuck_%d_%d",&robot_id,(int*)&state);

    emi_name.str("");
    emi_name << "emi" << robot_id;
    em = robot_get_device(emi_name.str().c_str());
    if (em==0) robot_console_printf("missing receiver %d\n",i);
    //emitter_enable(em,32);

    radio = robot_get_device("radio");
    radio_enable(radio,64);
    radio_set_tx_power(radio,0.008*COM_POWER);
    radio_set_callback(radio,radio_callback);
    printf("robot %d with tx_power: %f \n",robot_id,radio_get_tx_power(radio));

    srand(getpid()); // Seed random generator

    if (state == RANDOM) // Check initial state 
    {
        /* Generate random initial state */
        if (rnd() < RIGHT_WALL_PROB) state = RIGHT;
        else state = LEFT;
        printf("Robot %d with random selection: %d\n",robot_id,state);
    } 
    else 
    {
        printf("Robot %d with deterministic selection: %d\n",robot_id,state);
    }

    emitter_send_packet(em,&state,sizeof(char));

    lmsg = 0; rmsg = 0; // Clear message variables
}





static int run(int ms)
{
    static int clock = 0; 
    int d1,d2;                  // motor speed 1 and 2
    int msl,msr;                // motor speed left and right
    int distances[NB_SENSORS];  // array keeping the distance sensor readings
    int sensor_nb;           // FOR-loop counters
    RadioMessage msg;
    char m[4];

    d1=0; d2=0;                
    

        if ( clock % (int)(1024*DECISION_INTERVAL) == 64*(robot_id%10))
        {
            /* Check for opinion change */
            
            state = changeOpinion();
            
            //printf("%d: left=%d, right=%d\n",robot_id,lmsg,rmsg);
            
            lmsg = 0; rmsg = 0;
            
            m[0] = robot_id / 100;
            m[1] = (robot_id%100) / 10;
            m[2] = robot_id%10;
            m[3] = state;
            

            /* Broadcast local opinion */
            msg = radio_message_new(6,m,"broadcast");
            radio_send(radio,msg,0);
        }

        /* Print message received in the callback function */
        if (DEBUG && display[0]) {
            robot_console_printf("%s\n",display);
            display[0]=0; /* don't display twice */
        }

    for(sensor_nb=0;sensor_nb<NB_SENSORS;sensor_nb++)
    {  // read sensor values and calculate motor speeds
        /* Check for state */
        if (state == RIGHT)
            distances[sensor_nb] =
                distance_sensor_get_value(ds[sensor_nb]);
        else
            distances[sensor_nb] =
                distance_sensor_get_value(ds[NB_SENSORS-1-sensor_nb]);

        d1 += (distances[sensor_nb]-300) * Interconn[sensor_nb];
        d2 += (distances[sensor_nb]-300) * Interconn[sensor_nb + NB_SENSORS];
    }

    d1 /= 80; d2 /= 80;        // Normalizing speeds

    /* Check for state */
    if (state == RIGHT) { msr = d1+BIAS_SPEED; msl = d2+BIAS_SPEED; } 
    else                { msr = d2+BIAS_SPEED; msl = d1+BIAS_SPEED; }
      
    differential_wheels_set_speed(msl,msr);

    /*
    for(int i=0;i<9;i++)
    {
      if(table[i][2] > 0)
        table[i][2] -= 1;
      if(table[i][2] == 0)
        table[i][1] = 0;
    }
    */
        
    clock += 64;
    return 64;

}



int main()
{
    robot_live(reset);
    robot_run(run);     // never returns
    return 0; 
}  


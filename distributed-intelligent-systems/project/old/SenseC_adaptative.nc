#include "Timer.h"
#include "math.h"


module SenseC
{
  // Declare the interfaces used by this module
  uses {
    interface Boot;
    interface Leds;
    interface Timer<TMilli>;
    interface Read<uint16_t>;
    interface Mts300Sounder as Sounder;

	// used for sync
    interface GlobalTime<TMilli>;

    interface Packet;
    interface AMPacket;
    interface Queue<message_t> as MessageQueue;
    interface SplitControl as SerialControl;
    interface AMSend as SerialSender[am_id_t id];
  }
}
implementation
{
  uint32_t dt = 250; // Sampling interval in milliseconds
  bool sending = FALSE;
  message_t viz_msg;
  uint32_t tstamp;
  // YOU CAN DECLARE GLOBAL VARIABLES HERE
    //uint32_t dt2 = 500; //second sampling frequency 
    uint32_t seuil = 10: //a peu près la valeur des bruits les plus importants
	
  
  // First function called
  event void Boot.booted() {
    tstamp = 0;
    call Timer.startPeriodic(dt);
    call SerialControl.start();
  }

  // Interrupt function for the Timer
  event void Timer.fired() 
  {
    call Read.read();
  }

  task void send_next_message()
    {
        if (!sending && call MessageQueue.empty() == FALSE)
        {
            uint8_t payload_length;

            // send next message in queue
            viz_msg = call MessageQueue.head();
            payload_length = call Packet.payloadLength(&viz_msg);
            if (call SerialSender.send[AM_DATAMSG](AM_BROADCAST_ADDR, &viz_msg, payload_length) == SUCCESS)
            {
                sending = TRUE;
            }
            else
            {
                post send_next_message();
            }
        }
    }

  // Event called when a new reading is done
  event void Read.readDone(error_t result, uint16_t data) 
  {
    message_t new_msg;
    DataMsg *data_msg;
    // YOU CAN DECLARE LOCAL VARIABLES HERE
    //mettre les variables qui vont faire le test entre les 2 anciennes valeurs
    //en sortir une pente et une valeur attendue. 
    uint16_t* data_msg1;
	uint16_t* data_msg2;
	uint32_t v_expected;
	uint32_t* pointeur_dt = &dt;

    if (result == SUCCESS){
	  //call Sounder.beep(10);
	  call GlobalTime.getGlobalTime(&tstamp);

	  // build message payload
	  data_msg = (DataMsg*)(call Packet.getPayload(&new_msg, sizeof(DataMsg)));
	  data_msg->value = data;
	  data_msg->time = tstamp;

	  call Packet.setPayloadLength(&new_msg, sizeof(DataMsg));
	  call AMPacket.setType(&new_msg, AM_DATAMSG);
	  call MessageQueue.enqueue(new_msg);
	  post send_next_message();    

      // YOUR CODE STARTS HERE
	  // you need to implement a method for deciding on the sampling interval dt
	  //teste: si la difference entre la valeur attendue et la valeur obtenue est supérieure à un certain seuil 
	  //on augmente la fréquence de sampling
	  v_expected = (*data_msg1 - *data_msg2)/(*pointeur_dt) + *data_msg1;
		if (fabs(v_expected-data) > seuil){
			*pointeur_dt = *pointeur_dt-fabs(v_expected - data)*2;
			
			if(*pointeur_dt<250){
				*pointeur_dt=250;
			}			
		}
		else {
			*pointeur_dt = *pointeur_dt+fabs(v_expected - data)*2 ;
			
			if(*pointeur_dt>600){
				*pointeur_dt=600;
			}
		}
			call Timer.startPeriodic(dt);
    }
	
	*data_msg2 = &data_msg1;
	*data_msg1 = &data;
  }

  event void SerialSender.sendDone[am_id_t id](message_t* bufPtr, error_t error)
    {
	if (error == SUCCESS)
        {
            call MessageQueue.dequeue();
        }
	sending = FALSE;
        post send_next_message();
    }

  event void SerialControl.startDone(error_t err) { }
  event void SerialControl.stopDone(error_t err) { }
}

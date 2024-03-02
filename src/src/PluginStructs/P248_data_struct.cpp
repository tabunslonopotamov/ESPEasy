#include "../PluginStructs/P248_data_struct.h"

#ifdef USES_P248


/**************************************************************************
* Constructor
**************************************************************************/
P248_data_struct::P248_data_struct(struct EventStruct *event) {
  _pin1 = CONFIG_PIN1;
  _pin2 = CONFIG_PIN2;
}

/*****************************************************
* Destructor
*****************************************************/
P248_data_struct::~P248_data_struct() {
  if (initialised) {
    detachInterrupt(digitalPinToInterrupt(_pin1));
//    detachInterrupt(digitalPinToInterrupt(_pin2));
  }
}

/**************************************************************************
* plugin_init Initialize interrupt handling
**************************************************************************/
bool P248_data_struct::plugin_init(struct EventStruct *event) {
  if (validGpio(_pin1) && validGpio(_pin2)) {
    pinMode(_pin1, INPUT_PULLUP);
//    pinMode(_pin2, INPUT_PULLUP);

    attachInterruptArg(digitalPinToInterrupt(_pin1),
                       reinterpret_cast<void (*)(void *)>(Plugin_248_interrupt1),
                       this,
                       FALLING);
//    attachInterruptArg(digitalPinToInterrupt(_p2),
//                      reinterpret_cast<void (*)(void *)>(Plugin_248_interrupt2),
//                       this,
//                       FALLING);
    initialised = true;
  }
  return initialised;
}

/*****************************************************
* plugin_ten_per_second
*****************************************************/
bool P248_data_struct::plugin_ten_per_second(struct EventStruct *event) {
  bool success = false;

  if (initialised) {


        if (loglevelActiveFor(LOG_LEVEL_INFO)) {
          // write log
          String log = F("RFID : ");

//          if (new_key) {
//            log += F("New Tag: ");
//          } else {
//            log += F("Old Tag: ");
//          }
//          log += (unsigned long)keyBuffer;
//          log += F(", 0x");
//          log += ull2String(keyBuffer, 16);
//          log += F(", mask: 0x");
//          log += ull2String(keyMask, 16);
//          log += F(" Bits: ");
//          log += bitCount;
          addLogMove(LOG_LEVEL_INFO, log);
        };

        // reset everything
        keyBuffer    = 0ull;
        bitCount     = 0u;
        timeoutCount = 0u;

//        if (new_key) { sendData(event); }
//        resetTimer = 250; 
        Scheduler.setPluginTaskTimer(250, event->TaskIndex, event->Par1);

    }
  return success;
}

/*****************************************************
* plugin_timer_in
*****************************************************/
bool P248_data_struct::plugin_timer_in(struct EventStruct *event) {
  if (initialised) { // P248_AUTO_REMOVE check uses inversed logic!
    // Reset card id on timeout
    UserVar.setSensorTypeLong(event->TaskIndex, 777);
    bufferValid = true;
    //bufferBits  = P248_DATA_BITS;
    addLog(LOG_LEVEL_INFO, F("RFID : Removed Tag"));

 //   if (P248_REMOVE_EVENT == 1) {
      sendData(event);
//    }
    return true;
  }
  return false;
}


/*********************************************************************
* Interrupt 1 : Handle 1 bits
*********************************************************************/
void IRAM_ATTR P248_data_struct::Plugin_248_interrupt1(P248_data_struct *self) {

}

/********************************************************************
* plugin_get_config
********************************************************************/
bool P248_data_struct::plugin_get_config(struct EventStruct *event,
                                         String            & string) {
  bool success = false;

  if (initialised) {

  }

  return success;
}

#endif // ifdef USES_P248

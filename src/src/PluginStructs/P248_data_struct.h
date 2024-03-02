#ifndef PLUGINSTRUCTS_P248_DATA_STRUCT_H
#define PLUGINSTRUCTS_P248_DATA_STRUCT_H

#include "../../_Plugin_Helper.h"
#ifdef USES_P248

# define P248_CODELEN       PCONFIG(0)
# define P248_FLAG1   PCONFIG(1)

# define P248_TIMEOUT_LIMIT   5 // Number of loops through plugin_one_per_second = 5 second time-out

struct P248_data_struct : public PluginTaskData_base {
public:

  P248_data_struct(struct EventStruct *event);

  P248_data_struct() = delete;
  virtual ~P248_data_struct();

  bool plugin_init(struct EventStruct *event);
  bool plugin_ten_per_second(struct EventStruct *event);
  bool plugin_timer_in(struct EventStruct *event);
  bool plugin_get_config(struct EventStruct *event,
                         String            & string);

  volatile uint8_t  bitCount  = 0u;   // Count the number of bits received.
  volatile uint64_t keyBuffer = 0ull; // A 64-bit-long keyBuffer into which the number is stored.

private:

  uint64_t    castHexAsDec(uint64_t hexValue);

  static void Plugin_248_shift_bit_in_buffer(P248_data_struct *self,
                                             uint8_t           bit);
  static void Plugin_248_interrupt1(P248_data_struct *self);

  uint8_t timeoutCount = 0u;
  bool    initialised  = false;
  bool    bufferValid  = false;
  uint8_t bufferBits   = 0u;

  int8_t _pin1 = -1;
  int8_t _pin2 = -1;
};

#endif // ifdef USES_P248
#endif // ifndef PLUGINSTRUCTS_P248_DATA_STRUCT_H

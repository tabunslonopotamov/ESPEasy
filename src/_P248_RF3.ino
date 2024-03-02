#include "_Plugin_Helper.h"
#ifdef USES_P248

// #######################################################################################################
// ################################# Plugin 248: RF3 #################################
// #######################################################################################################


# define PLUGIN_248
# define PLUGIN_ID_248         8
# define PLUGIN_NAME_248       "Generic - RF3"
# define PLUGIN_VALUENAME1_248 "sn"

# include "src/PluginStructs/P248_data_struct.h"


boolean Plugin_248(uint8_t function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
    {
      Device[++deviceCount].Number           = PLUGIN_ID_248;
      Device[deviceCount].Type               = DEVICE_TYPE_DUAL;
      Device[deviceCount].VType              = Sensor_VType::SENSOR_TYPE_ULONG;
      Device[deviceCount].Ports              = 0;
      Device[deviceCount].PullUpOption       = false;
      Device[deviceCount].InverseLogicOption = false;
      Device[deviceCount].FormulaOption      = false;
      Device[deviceCount].ValueCount         = 1;
      Device[deviceCount].SendDataOption     = true;
      Device[deviceCount].TimerOption        = false;
      Device[deviceCount].GlobalSyncOption   = true;
      break;
    }

    case PLUGIN_GET_DEVICENAME:
    {
      string = F(PLUGIN_NAME_248);
      break;
    }

    case PLUGIN_GET_DEVICEVALUENAMES:
    {
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_248));
      break;
    }

    case PLUGIN_GET_DEVICEGPIONAMES:
    {
      event->String1 = formatGpioName_input(F("RX Pin"));
      event->String2 = formatGpioName_input(F("TX Pin"));
      break;
    }

    case PLUGIN_SET_DEFAULTS:
    {
      P248_CODELEN      = 26; 
      P248_FLAG1  = 1;   
      break;
    }

    case PLUGIN_INIT:
    {
      initPluginTaskData(event->TaskIndex, new (std::nothrow) P248_data_struct(event));
      P248_data_struct *P248_data = static_cast<P248_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P248_data) {
        success = P248_data->plugin_init(event);
      }
      break;
    }

    case PLUGIN_TASKTIMER_IN:
    {
      P248_data_struct *P248_data = static_cast<P248_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P248_data) {
        success = P248_data->plugin_timer_in(event);
      }
      break;
    }

    case PLUGIN_TEN_PER_SECOND:
    {
      P248_data_struct *P248_data = static_cast<P248_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P248_data) {
        success = P248_data->plugin_ten_per_second(event);
      }
      break;
    }

    case PLUGIN_GET_CONFIG_VALUE:
    {
      P248_data_struct *P248_data = static_cast<P248_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P248_data) {
        success = P248_data->plugin_get_config(event, string);
      }
      break;
    }

    case PLUGIN_WEBFORM_LOAD:
    {
      addFormCheckBox(F("Parament flag1"), F("flag1"), P248_FLAG1 == 0);
      # ifndef BUILD_NO_DEBUG
      addFormNote(F("Earlier versions of this plugin have used GPIO pins inverted, giving different Tag results."));
      # endif // ifndef BUILD_NO_DEBUG

      addFormNumericBox(F("Code length (press)"), F("codelen"), P248_CODELEN, 3, 5);
      addUnit(F("3..5 press"));
      # ifdef BUILD_NO_DEBUG
      addFormNote(F("Select activation code lengh 3 4 5 button press"));
      # endif // ifdef BUILD_NO_DEBUG

      success = true;
      break;
    }

    case PLUGIN_WEBFORM_SAVE:
    {
      P248_CODELEN      = getFormItemInt(F("codelen"));
      P248_FLAG1 = isFormItemChecked(F("flag1")) ? 0 : 1;    // Inverted logic!

      // uint64_t keyMask = 0LL;
      // keyMask = (0x1ull << (P248_DATA_BITS - 2));
      // keyMask--;
      // String log = F("P248: testing keyMask = 0x");
      // log += ull2String(keyMask, HEX);
      // log += F(" bits: ");
      // log += P248_DATA_BITS;
      // addLog(LOG_LEVEL_INFO, log);

      success = true;
      break;
    }
  }
  return success;
}

#endif // USES_P248

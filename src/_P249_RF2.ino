#define rxPin 4        // сюда у нас подключен приемник

#include "_Plugin_Helper.h"
#ifdef USES_P249


// number and name of counted values
# define PLUGIN_NR_VALUENAMES_249       4
# define PLUGIN_VALUENAME1_249          "lastSN"
# define PLUGIN_VALUENAME2_249          "lastSeq"
# define PLUGIN_VALUENAME3_249          "btn"
# define PLUGIN_VALUENAME4_249          "bLow"
# define PLUGIN_VALUENAME5_249          "btn2"
# define PLUGIN_VALUENAME6_249          "btn3"
# define PLUGIN_VALUENAME7_249          "bLow"
# define PLUGIN_VALUENAME8_249          "ovr1"
# define PLUGIN_VALUENAME9_249          "ovr2"
# define PLUGIN_VALUENAME10_249          "res"


volatile long rfserial;
volatile long iRepeat ;
volatile long bLow ;
volatile long s2  ;
volatile long s1  ;
volatile long s0 ;
volatile long s3 ;
volatile long btn ;
volatile long seqnum;
volatile long ovr1;
volatile long ovr2;
volatile long prevCheck;



// #######################################################################################################
// #################################### Plugin 249: RF2 ################################################
// #######################################################################################################

volatile unsigned long P249_num1 = 0;
void    Plugin_249_interrupt() IRAM_ATTR;

#define Pe 413         //DoorHan
#define Pe2 Pe * 2     //DoorHan
#define MAX_DELTA 200  //Максимальное отклонение от длительности при приеме

volatile static uint64_t manufacturer = 0x8455F43584941223;  //тут надо поставить мануфактурник DoorHan

volatile byte levelrf = 255;
volatile unsigned long last, len;
volatile unsigned int r = 0;
volatile unsigned int lolen, hilen;
//volatile unsigned int ii=0;
volatile static byte bcounter = 0;  // количество принятых битов
volatile static long code1 = 0;     // зашифрованная часть
volatile static long code2 = 0;     // фиксированная часть
volatile static long Cache1 = 0;    // зашифрованная часть
volatile static long Cache2 = 0;    // фиксированная часть
volatile static long Cache1R = 0;   // зашифрованная часть
volatile static long Cache2R = 0;   // зашифрованная часть
volatile static long Cache11 = 0;   // зашифрованная часть
volatile static long Cache22 = 0;   // фиксированная часть
volatile static long Cache23 = 0;   // фиксированная часть
volatile static long Cache1O = 0;   // зашифрованная часть
volatile static long Cache2O = 0;   // фиксированная часть


///////////////кусок из Wiki функции кодирования и декодирования Keeloq
#define KeeLoq_NLF 0x3A5C742E
#define bitn(x, n) (((x) >> (n)) & 1)
#define g5(x, a, b, c, d, e) (bitn(x, a) + bitn(x, b) * 2 + bitn(x, c) * 4 + bitn(x, d) * 8 + bitn(x, e) * 16)

uint32_t KeeLoq_Encrypt(const uint32_t data, const uint64_t key) {
  uint32_t x = data, r;
  for (r = 0; r < 528; r++)
    x = (x >> 1) ^ ((bitn(x, 0) ^ bitn(x, 16) ^ (uint32_t)bitn(key, r & 63) ^ bitn(KeeLoq_NLF, g5(x, 1, 9, 20, 26, 31))) << 31);
  return x;
}

uint32_t KeeLoq_Decrypt(const uint32_t data, const uint64_t key) {
  uint32_t x = data, r;
  for (r = 0; r < 528; r++)
    x = (x << 1) ^ bitn(x, 31) ^ bitn(x, 15) ^ (uint32_t)bitn(key, (15 - r) & 63) ^ bitn(KeeLoq_NLF, g5(x, 0, 8, 19, 25, 30));
  return x;
}

boolean CheckValue(unsigned int base, unsigned int value) {
  return ((value == base) || ((value > base) && ((value - base) < MAX_DELTA)) || ((value < base) && ((base - value) < MAX_DELTA)));
}

//////////////////функция зеркалирования кода, для дальнейшего декодирования.
long inver(long Cache345) {
  long Cache2RR;
  for (int i = 0; i < 32; i++) {
    bitWrite(Cache2RR, i, bitRead(Cache345, 31 - i));
  }
  return Cache2RR;
}
//////////////////////////////////////////////////////




# define PLUGIN_249
# define PLUGIN_ID_249         249
# define PLUGIN_NAME_249       "Generic - RF2"
boolean Plugin_249(uint8_t function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
    {
      Device[++deviceCount].Number           = PLUGIN_ID_249;
      Device[deviceCount].Type               = DEVICE_TYPE_DUAL;
      Device[deviceCount].VType              = Sensor_VType::SENSOR_TYPE_DUAL;
      Device[deviceCount].Ports              = 0;
      Device[deviceCount].PullUpOption       = false;
      Device[deviceCount].InverseLogicOption = false;
      Device[deviceCount].FormulaOption      = false;
      Device[deviceCount].DecimalsOnly       = true;
      Device[deviceCount].ValueCount         = 4;
      Device[deviceCount].SendDataOption     = true;
      Device[deviceCount].TimerOption        = true;
      Device[deviceCount].TimerOptional      = true;
      Device[deviceCount].GlobalSyncOption   = true;
      Device[deviceCount].OutputDataType     = Output_Data_type_t::All;
      Device[deviceCount].PluginStats        = true;
      break;
    }

    case PLUGIN_GET_DEVICENAME:
    {
      string = F(PLUGIN_NAME_249);
      break;
    }

    case PLUGIN_GET_DEVICEVALUENAMES:
    {
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_249));
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_249));
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_249));
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[3], PSTR(PLUGIN_VALUENAME4_249));
//      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[4], PSTR(PLUGIN_VALUENAME5_249));
//     strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[5], PSTR(PLUGIN_VALUENAME6_249));
//      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[6], PSTR(PLUGIN_VALUENAME7_249));
//      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[7], PSTR(PLUGIN_VALUENAME8_249));
//      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[8], PSTR(PLUGIN_VALUENAME9_249));
//      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[9], PSTR(PLUGIN_VALUENAME10_249));

      break;
    }

//    case PLUGIN_GET_DEVICEVALUECOUNT:
//    {
//      event->Par1 = getValueCountFromSensorType(static_cast<Sensor_VType>3);
//      event->Par1 = getValueCountFromSensorType(static_cast<Sensor_VType>(3));
//      success     = true;
//      break;
//    }

    case PLUGIN_GET_DEVICEVTYPE:
    {
      event->sensorType = static_cast<Sensor_VType>(10);;
      event->idx        = 0;
      success           = true;
      break;
    }

    case PLUGIN_INIT:
    {
        pinMode(rxPin, INPUT);
        attachInterrupt(rxPin, Plugin_249_interrupt , CHANGE);
      success = true;
      break;
    }

    case PLUGIN_READ:
    {
            event->sensorType                = Sensor_VType::SENSOR_TYPE_TRIPLE;
           UserVar[event->BaseVarIndex]     = rfserial;
           UserVar[event->BaseVarIndex + 1]     = seqnum;
           UserVar[event->BaseVarIndex + 2]     = btn;
           UserVar[event->BaseVarIndex + 3]     = bLow;
//           UserVar[event->BaseVarIndex + 4]     = s2;
//           UserVar[event->BaseVarIndex + 5]     = s3;
//           UserVar[event->BaseVarIndex + 6]     = bLow;
//           UserVar[event->BaseVarIndex + 7]     = ovr1;
//           UserVar[event->BaseVarIndex + 8]     = ovr2;
//           UserVar[event->BaseVarIndex + 9]     = s0;

      //event->sensorType = static_cast<Sensor_VType>(3);

      if (loglevelActiveFor(LOG_LEVEL_INFO)) {
        for (uint8_t x = 0; x < 10; x++)
        {
          String log = F("RF2: value ");
          log += x + 1;
          log += F(": ");
          log += formatUserVarNoCheck(event->TaskIndex, x);
          addLogMove(LOG_LEVEL_INFO, log);
        }
      }
      success = true;
      break;
    }

    case PLUGIN_TEN_PER_SECOND:
    {
  if (r == 1)  //Если поймали чего выводим в терминал.
  {
//    Serial.print(Cache1, HEX);  //вывод ключа в виде как поймали часть 1
//    Serial.print(":");
//    Serial.print(Cache2, HEX);  //вывод ключа в виде как поймали часть 2
//    Serial.print(" ");
//    Serial.print(KeeLoq_Decrypt(inver(Cache1), manufacturer), HEX);  //Расшифрованная часть ключа 1
//    Serial.print(":");
//    Serial.println(inver(Cache2), HEX);  //открытая (незашифрованная) часть ключа часть 2

//    Serial.print(" ");
//   Serial.print(KeeLoq_Decrypt(inver(Cache1), manufacturer), BIN);  //Расшифрованная часть ключа 1
//    Serial.print(":");
//   Serial.println(inver(Cache2), BIN);  //открытая (незашифрованная) часть ключа часть 2

    prevCheck = rfserial+seqnum+btn;
    long p1= KeeLoq_Decrypt(inver(Cache1), manufacturer) ;
    long p2=inver(Cache2);
    rfserial = p2 & 0b1111111111111111111111111111;
    iRepeat = bitRead(p2, 33);
    bLow = bitRead(p2, 32);
    s2 = bitRead(p1, 31);
    s1 = bitRead(p1, 30);
    s0 = bitRead(p1, 29);
    s3 = bitRead(p1, 28);
    seqnum = p1 & 0b111111111111111111111111;
    ovr1=bitRead(p1, 26);
    ovr2=bitRead(p1, 27);
    btn=s0+2*s1+4*s2+8*s3;
//    String log = F("prevCheck: ");
//    log += prevCheck;
//    log += F(" new value: ");
//    log += rfserial+seqnum+btn;
//    addLogMove(LOG_LEVEL_INFO, log);
    if ( rfserial+seqnum+btn == prevCheck ) { r = 0; break; }
    else {
//    SerialPrintf("serial: %d bLow: %d , s0: %d, s1: %d , s2: %d , s3: %d , seqnum: %d, ovr1: %d, ovr2: %d , iRepeat: %d \n", serial,bLow,s0,s1,s2,s3,seqnum,ovr1,ovr2,iRepeat);
            String log = F("hcs301 serial: ");
            log += rfserial;
            log += F(" seqnum: ");
            log += seqnum;
            log += F(" s0: ");
            log += s0;
            log += F(" s1: ");
            log += s1;
            log += F(" s2: ");
            log += s2;
            log += F(" s3: ");
            log += s3;
            log += F(" ovr1: ");
            log += ovr1;
            log += F(" ovr2: ");
            log += ovr2;
            log += F(" bLow: ");
            log += bLow;
            log += F(" iRepeat: ");
            log += iRepeat;

//            addLogMove(LOG_LEVEL_INFO, log);
//          log  = F("BMP  : Barometric Pressure: ");
//            log += formatUserVarNoCheck(event->TaskIndex, 1);
            addLogMove(LOG_LEVEL_INFO, log);
//            controller_send (1, "bla2","bla2");
//            sendData(event);
//            UserVar.setSensorTypeLong(event->TaskIndex, rfserial);
//            UserVar.setSensorTypeLong(event->TaskIndex+1, seqnum);
 //           UserVar.setSensorTypeLong(event->TaskIndex+2, s0);
            event->sensorType                = Sensor_VType::SENSOR_TYPE_TRIPLE;
           UserVar[event->BaseVarIndex]     = rfserial;
           UserVar[event->BaseVarIndex + 1]     = seqnum;
           UserVar[event->BaseVarIndex + 2]     = btn;
           UserVar[event->BaseVarIndex + 3]     = bLow;
//           UserVar[event->BaseVarIndex + 4]     = s2;
//           UserVar[event->BaseVarIndex + 5]     = s3;
//           UserVar[event->BaseVarIndex + 6]     = bLow;
//          UserVar[event->BaseVarIndex + 7]     = ovr1;
//           UserVar[event->BaseVarIndex + 8]     = ovr2;
//           UserVar[event->BaseVarIndex + 9]     = s0;

           sendData(event);
          };
//    Serial.print(seri);  //открытая (незашифрованная) часть ключа часть 2
    //ну а это для проверки что все красиво работает прибавим счетчик и зашифруем обратно
    //Serial.print(inver(KeeLoq_Encrypt(KeeLoq_Decrypt(inver(Cache1),manufacturer)+1,manufacturer)),HEX);
    //Serial.print(":");
    //Serial.println(Cache2, HEX);//вывод ключа в виде как поймали часть 2

    //SendANMotors(inver(KeeLoq_Encrypt(KeeLoq_Decrypt(inver(Cache1),manufacturer)+1,manufacturer)), Cache2);//Посылаем в эфир то что поймали, расшифровали, прибавили счетчик, зашифровали.

    //SendANMotors(0xffffffff, 0xffffffff);//можно если ключик статичен слать и так
    /////////////////////////////////////////////////////////////////////////////////////
    r = 0;
  }

      success = true;
      break;
    }


  }
  return success;
}


void Plugin_249_interrupt() {

  //portENTER_CRITICAL();
//  ISR_noInterrupts();

  //noInterrupts();
  len = micros() - last;
  last = micros();
//  portEXIT_CRITICAL();
  //ISR_interrupts();
  //portDISABLE_INTERRUPTS();
  //portENABLE_INTERRUPTS();

  //interrupts();
if (digitalRead(rxPin) == HIGH) {
    levelrf = 1;
    lolen = len;
  } else {
    levelrf = 0;
    hilen = len;
  }

  ///////////////////Прием Doorhan и ему подобные
  if (levelrf == 1) {
    if (CheckValue(Pe, hilen) && CheckValue(Pe2, lolen)) {
      if (bcounter < 32)
        code1 = (code1 << 1) | 1;
      else if (bcounter < 64)
        code2 = (code2 << 1) | 1;
      bcounter++;
    } else if (CheckValue(Pe2, hilen) && CheckValue(Pe, lolen)) {  // valid 0
      if (bcounter < 32)
        code1 = (code1 << 1) | 0;
      else if (bcounter < 64)
        code2 = (code2 << 1) | 0;
      bcounter++;
    } else
      bcounter = 0;
    //Поймали 64 бита, значит пишем их в переменные для дальнейшей работы с ними (пол дела сделано!)
    if (bcounter >= 65) {
      Cache1 = code1;  //первый кусок кода (зашифрованный)
      Cache2 = code2;  //второй кусок кода (открытый)
      bcounter = 0;

      r = 1;  //Это означает что поймали ключик и можно с ним работать
      code1 = 0;
      code2 = 0;
    }
  }

}
#endif // USES_p249

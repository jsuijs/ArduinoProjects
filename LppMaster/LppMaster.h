
#ifdef __ROBOTLIB_CPP_H
   #define LPP_I2C_ADDRESS 0xD0
   typedef int lpp_int;
   typedef char lpp_tx_buffer;
   typedef char lpp_rx_buffer;

#else
   #include <arduino.h>

   #define LPP_I2C_ADDRESS 0x68
   typedef short lpp_int;
   typedef const byte lpp_tx_buffer;
   typedef       byte lpp_rx_buffer;


   bool I2cSendReceive(byte I2cSlaveAddress, byte TxCount, byte RxCount, const byte *TxBuffer, byte *RxBuffer);
   void HexDump( const void *Data, unsigned int Length, unsigned int Offset=0);
   bool I2cWrite_Byte_Byte(int I2cSlaveAddress, int RegAddr, int Data);
   bool I2cWrite_Byte_Word(int I2cSlaveAddress, int RegAddr, int Data);
   int NormDegrees(int Degrees);
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// The three datastructs of the class
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct TLppSensorData
{
   lpp_int Distance;    // mm
   lpp_int Degrees32;   // degrees * 32
};

//-----------------------------------------------------------------------------
struct TLppArrayData
{
   lpp_int Distance;    // mm
};

//-----------------------------------------------------------------------------
struct TLppStatusData
{
   char ID;             // 0
   char SwRevision;     // 1
   char RotationCount;  // 2
   char RotationTime;   // 3 - in ms
   char free04;         // 4
   char free05;         // 5
   char free06;         // 6
   char free07;         // 7
   char OffsetX;        // 8 - in 2mm steps
   char OffsetY;        // 9 - in 2mm steps
   char Cmd;            // 10 (just about always zero....)
   char RotSpeed;       // 11
   short OffsetAngle; // 12 & 13 - in degrees
   char Reverse;        // 14
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class TLpp {
  public:
   TLpp();
   void  Takt();
   TLppSensorData   Sensor[8];
   TLppArrayData    Array[16];
   TLppStatusData   Status;

   bool begin();
   bool Start();
   bool Stop();
   bool SetReverse(char TrueFalse) { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, 14, TrueFalse); };
   bool SetOffsetDegrees(int Degrees)  { return I2cWrite_Byte_Word(LPP_I2C_ADDRESS, 12, Degrees);     };
   bool SetOffsetX(int Mm)             { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, 8,  Mm / 2);      };
   bool SetOffsetY(int Mm)             { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, 9,  Mm / 2);      };

   bool ArraySetup(int StartAngle, int StepAngle, int StepCount);
   bool SensorSetup(int Nr, int StartAngle, int StepAngle);

   bool ReadStatus();
   bool ReadArray();
   bool ReadSensors(int Count = 8);

   bool IsRunning();
   void PrintStatus();
   void PrintArray();
   void PrintSensors();

   void ReadPrintSensorCfg(int Nr);
  private:
   int EnableMode;   // 0 = begin not called (or no succes), 1 = inactive, 2 = active
   bool _SA_Setup(char StartIx, char Mode, int StartAngle, int StepAngle, int StepCount);
   bool _ReadShorts(char StartIx, char NrOfShorts, lpp_int *Data);

    int ArrayCount;   // set by ArraySetup()
    int SensorCount;  // set by (last) ReadSensors()
} ;

extern TLpp Lpp;

// --- C++ ---

//-----------------------------------------------------------------------------
// TLpp::TLpp - constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TLpp::TLpp()
   {

#ifdef __ROBOTLIB_CPP_H
#ifndef RLM_AUTO_CLI_ADD_DISABLE
      CliAddCommands(CliLpp, "Lpp");
#endif
  #ifndef RLM_BASIC_DISABLE
      BasicSymbols.AddDispatchFunction("Lpp.Start", "", 1, 0, (void *)LppStartStop);
      BasicSymbols.AddDispatchFunction("Lpp.Stop",  "", 0, 0, (void *)LppStartStop);
  #endif
      Registry.Add("Lpp.Array.Distance",   LppReadArrayDistance,   0);
      Registry.Add("Lpp.Sensor.Distance",  LppReadSensorDistance,  0);
      Registry.Add("Lpp.Sensor.HrDegrees", LppReadSensorDegrees32, 0);
#endif

      ArrayCount  = 16; // default = max
      SensorCount = 8;  // default = max
      EnableMode  = 0;
   }
//-----------------------------------------------------------------------------
// TLpp::begin - startup checks
//-----------------------------------------------------------------------------
// return true on succes.
//-----------------------------------------------------------------------------
bool TLpp::begin()
   {  bool r;
      if (EnableMode != 0) return true;
      // some tries to communicate, allows Lpp bootloader timeout.
      for (int i=0; i< 20; i++) {
         r = ReadStatus();
         if (r == true) {
            // check here for version, sometimes we get the wrong one on startup
            if (Status.SwRevision != 2) {
               printf("Lpp.begin() failed (sw version error - %d)\n", (int) Status.SwRevision);
               delay(250);
               continue;
      }
            break;
      }
         delay(250);
      }
      if (r == false) {
         printf("Lpp.begin() failed (i2c error)\n");
         return false;
      }
      EnableMode = 1;
      return true;
   }
//-----------------------------------------------------------------------------
// TLpp::Start - Activate lpp communications
//-----------------------------------------------------------------------------
// return true on succes.
//-----------------------------------------------------------------------------
bool TLpp::Start()
   {
      if (EnableMode == 0) {
         printf("Lpp.Start() error: EnableMode 0\n");
         return false;  // begin not called, of failure
      }
      EnableMode = 2;   // active
      return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, 10, 1);
   }
//-----------------------------------------------------------------------------
// TLpp::Stop - Suspend lpp communications
//-----------------------------------------------------------------------------
// return true on succes.
//-----------------------------------------------------------------------------
bool TLpp::Stop()
   {
      if (EnableMode == 0) return false;  // begin not called, of failure
      EnableMode = 1;   // inactive
      return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, 10, 2);
   }
//-----------------------------------------------------------------------------
// TLpp::IsRunning - check if lidar is running (scanning)
//-----------------------------------------------------------------------------
// return true when lidar is running.
//-----------------------------------------------------------------------------
bool TLpp::IsRunning()
   {
      // check if we have comms & master tried to start the lidar
      if (EnableMode != 2) return false;
      // check if lidar is rotating
      ReadStatus();
      if (Lpp.Status.RotationTime == 0) return false;
      return true;
   }
//-----------------------------------------------------------------------------
// TLpp::Takt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TLpp::Takt()
   {
      if (EnableMode != 2) return;
      ReadArray();
      ReadSensors();
   }
//-----------------------------------------------------------------------------
// TLpp::ArraySetup - Setup array with up to 16 segments.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TLpp::ArraySetup(int StartAngle, int StepAngle, int StepCount)
{
   ArrayCount = StepCount;
   if (ArrayCount > 16) ArrayCount = 15;
   return _SA_Setup(16, 1, StartAngle, StepAngle, ArrayCount);
}

//-----------------------------------------------------------------------------
// TLpp::SensorSetup - Setup sensor 0...7
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TLpp::SensorSetup(int Nr, int StartAngle, int StepAngle)
{
  return _SA_Setup(24 + Nr * 8, 3, StartAngle, StepAngle, 0);
}

//-----------------------------------------------------------------------------
// TLpp::ReadStatus - read status data from Lpp and store in class struct
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TLpp::ReadStatus()
   { char TxBuffer[7];

  TxBuffer[0] = 0;
      bool r = I2cSendReceive(LPP_I2C_ADDRESS, 1, sizeof(Status), (lpp_tx_buffer *)TxBuffer, (lpp_rx_buffer *)&Status);
      // swap bytes of short.
      unsigned int t = Status.OffsetAngle;
      Status.OffsetAngle = ((t & 0xFF) << 8) |  ((t >> 8) & 0xFF);
      return r;
}

//-----------------------------------------------------------------------------
// TLpp::ReadArray - read Array data from Lpp and store in class struct
//-----------------------------------------------------------------------------
// Note: Number of segments read is determined by ArrayCount, which is set
//       by ArraySetup().
//-----------------------------------------------------------------------------
bool TLpp::ReadArray()
{
      return _ReadShorts(88, ArrayCount, (lpp_int *) Array);
}

//-----------------------------------------------------------------------------
// TLpp::ReadSensors - Read Sensor data from Lpp and store in class struct array
//-----------------------------------------------------------------------------
// Note: Count can be used to read less then 8 sensors, when not all sensor
//       data is required. This saves time and i2c bandwith.
//-----------------------------------------------------------------------------
bool TLpp::ReadSensors(int Count)
{
    SensorCount = Count;
    if (SensorCount > 8) SensorCount = 8;
      return _ReadShorts(120, SensorCount * 2, (lpp_int *) Sensor);
   }
//-----------------------------------------------------------------------------
// TLpp::PrintStatus -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TLpp::PrintStatus()
   {
      printf("LidarID: %d, Sw: %d, RotCount: %d, RotTime: %d\n",
         Status.ID, Status.SwRevision, Status.RotationCount, Status.RotationTime);
      printf("Cfg RotSpeed: %d, OffsetAngle: %d, Reverse: %d\n",
         Status.RotSpeed, Status.OffsetAngle, Status.Reverse);
}

//-----------------------------------------------------------------------------
// TLpp::PrintArray -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TLpp::PrintArray()
{
      if (EnableMode != 2) printf("Lpp.PrintArray() warning: EnableMode != 2\n");
  for (int i=0; i<ArrayCount; i++) {
    printf("%4d ", Array[i].Distance);
  }
  printf("\n");
}

//-----------------------------------------------------------------------------
// TLpp::PrintSensors -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TLpp::PrintSensors()
{
      if (EnableMode != 2) printf("Lpp.PrintSensors() warning: EnableMode != 2\n");
  for (int i=0; i<SensorCount; i++) {
         printf("%d Distance: %d, Degrees: %d (%d)\n", i, Sensor[i].Distance, NormDegrees(Sensor[i].Degrees32 / 32), Sensor[i].Degrees32);
  }
}

//-----------------------------------------------------------------------------

// TLpp::ReadPrintSensorCfg -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TLpp::ReadPrintSensorCfg(int Nr)
   {  char TxBuffer[1];
      char RxBuffer[6];

      if ((Nr < -1) || (Nr > 7)) {
         printf("Error: sensor # out of range (%d), 0..7 for sensors, -1 for array.\n", Nr);
         return;
      }

      TxBuffer[0] = 24 + Nr * 8;
      I2cSendReceive(LPP_I2C_ADDRESS, 1, 6, (lpp_tx_buffer *)TxBuffer, (lpp_rx_buffer *)RxBuffer);

      short TmpStart = (((int)RxBuffer[2]) << 8) + RxBuffer[3];
      printf("Sensor %d, Mode: %d, Count: %d, Start: %d, Step: %d\n",
         Nr, RxBuffer[0], RxBuffer[1], TmpStart, (((int)RxBuffer[4]) << 8) + RxBuffer[5]);
   }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Support and private routines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// TLpp::_SA_Setup - private support routine for Sensor and Array setup
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TLpp::_SA_Setup(char StartIx, char Mode, int StartAngle, int StepAngle, int StepCount)
   { char TxBuffer[7];

  TxBuffer[0] = StartIx;
  TxBuffer[1] = Mode;
  TxBuffer[2] = StepCount;
  TxBuffer[3] = StartAngle >> 8;
  TxBuffer[4] = StartAngle & 0xFF;
  TxBuffer[5] = StepAngle >> 8;
  TxBuffer[6] = StepAngle & 0xFF;
      bool ret = I2cSendReceive(LPP_I2C_ADDRESS, 7, 0, (lpp_tx_buffer *)TxBuffer, NULL);
      delay(5);
      return ret;
}

//-----------------------------------------------------------------------------
// TLpp::_ReadShorts - private support routine - read shorts & swap bytes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TLpp::_ReadShorts(char StartIx, char NrOfShorts, lpp_int *Data)
   {  char TxBuffer[1];
#ifdef __ROBOTLIB_CPP_H
      // Data is array of ints, use separate buffer for temporary storage
      char RxBuffer[NrOfShorts * 2];
#else
      // Data is array of shorts, use it for temporary storage
      byte *RxBuffer = (byte *) Data;
#endif

  TxBuffer[0] = StartIx;
      bool r = I2cSendReceive(LPP_I2C_ADDRESS, 1, NrOfShorts * 2, (lpp_tx_buffer *)TxBuffer, RxBuffer);

//  HexDump((const void *)RxBuffer, 32);

  for (int i=0; i < NrOfShorts; i++) {
         Data[i] = (((int)RxBuffer[i*2]) << 8) + (int) (RxBuffer[i*2+1] & 0xFF);
  }

//  HexDump((const void *)Data, 32);
 return r;
}



//# cblock 1 utilities
//-----------------------------------------------------------------------------
// I2cWrite_Byte_Byte - Send a Byte address, then write a Byte of data.
//-----------------------------------------------------------------------------
// return: true on succes
//-----------------------------------------------------------------------------
bool I2cWrite_Byte_Byte(int I2cSlaveAddress, int RegAddr, int Data)
{  char TxBuffer[2];

   TxBuffer[0] = (RegAddr) & 0xFF;
   TxBuffer[1] = (Data   ) & 0xFF;
   return I2cSendReceive(I2cSlaveAddress, 2, 0, (const unsigned char *)TxBuffer, NULL);
}

//# cblock 1 utilities
//-----------------------------------------------------------------------------
// I2cWrite_Byte_Word - Send a Byte address, then write a Word of data.
//-----------------------------------------------------------------------------
// return: true on succes
//-----------------------------------------------------------------------------
bool I2cWrite_Byte_Word(int I2cSlaveAddress, int RegAddr, int Data)
{  char TxBuffer[3];

   TxBuffer[0] = (RegAddr  ) & 0xFF;
   TxBuffer[1] = (Data >> 8) & 0xFF;
   TxBuffer[2] = (Data     ) & 0xFF;
   return I2cSendReceive(I2cSlaveAddress, 3, 0, (const unsigned char *)TxBuffer, NULL);
}

//# cblock 1 utilities
//----------------------------------------------------------------------------
// NormDegrees - Norm angle in degrees, signed result (-179...180 degrees)
//----------------------------------------------------------------------------
// Param
//    degrees = input,
//
// Return: normalized degrees
//
// See NormAngle() for float (RAD) version.
//----------------------------------------------------------------------------
int NormDegrees(int Degrees)
{
   Degrees %= 360;  // step 1: +/- 360 degrees
   while (Degrees >   180) Degrees -= 360;   // > 180 degrees
   while (Degrees <= -180) Degrees += 360;   // =< -180 degrees (<=, not < because then both 180 and -180 would be valid)
   return Degrees;
}
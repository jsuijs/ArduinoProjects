// <FROM LIDER PREPOCESS SKETCH>
      //--------------------
      // I2c registers START
      //--------------------

      #define  INTERFACE_VERSION (4)
      // status values
      #define R_ID                  (        0)   // fixed number
      #define R_IF_REVISION         (        1)   // Interface revision # (INTERFACE_REVISION)
      #define R_ROTATION            (        2)   // rotation counter
      #define R_ROTTIME             (        3)   // rotation time (ms)
      #define R_SAMPLERATE_H        (        4)   // # of samples in last 100 ms
      #define R_SAMPLERATE_L        (        5)   // # of samples in last 100 ms
      // 6
      #define R_CMD                 (        7)   // command register (see main sketch)
      // setup values
      #define R_ROTSPEED            (        8)   // PWM output to control Lidar Rotation Rate
                                                  // NOTE: RotSpeed does not work linear! Due to pwm frequency??
      #define R_OFFSET_X            (        9)   // signed, in 2mm steps
      #define R_OFFSET_Y            (       10)   // signed, in 2mm steps
      #define R_OFFSET_DEGREES_H    (       11)   // Offset of Lidar zero-angle in degrees
      #define R_OFFSET_DEGREES_L    (       12)   // -
      #define R_REVERSE_ROTATION    (       13)   // true or false, reverse angular direction of Lidar output (360 - phi)
      //14
      //15
      #define R_CAN_SAMPLES         (       16)   // min # of samples for CAN detection
      #define R_CAN_EDGE            (       17)   // min edge size (delta between two samples, in mm) for CAN start/end
      #define R_CAN_MIN             (       18)   // min angle of can (units tbd)
      #define R_CAN_MAX             (       19)   // max angle of can (units tbd)
      // free up to SensorDataOffset
      #define SDO (32) // sensor data offset
      // SensorArray
      #define R_A_MODE              (SDO +  0)  // Array Mode
      #define R_A_STEP_COUNT        (SDO +  1)  // number of segments computed (note: only 15 pairs of bytes can be read at once!)
      #define R_A_START_DEGREES_H   (SDO +  2)  // Start of first segment
      #define R_A_START_DEGREES_L   (SDO +  3)  //
      #define R_A_STEP_DEGREES_H    (SDO +  4)  // size of one segment
      #define R_A_STEP_DEGREES_L    (SDO +  5)  //
      // 8 virtual sensors
      #define R_V0_MODE             (SDO +  8)  // Virtual sensor 0 Mode
      #define R_V0_SPARE            (SDO +  9)  // (spare register, keeps registers similar to SensorArray)
      #define R_V0_START_DEGREES_H  (SDO + 10)  // Start of sensor range
      #define R_V0_START_DEGREES_L  (SDO + 11)  //
      #define R_V0_STEP_DEGREES_H   (SDO + 12)  // Size of sensor range
      #define R_V0_STEP_DEGREES_L   (SDO + 13)  //
                                                // + 14 spare
                                                // + 15 spare
      #define R_V1_MODE             (SDO + 16)  // Virtual sensor 1 Mode
      #define R_V1_SPARE            (SDO + 17)  // (spare register, keeps registers similar to SensorArray)
      #define R_V1_START_DEGREES_H  (SDO + 18)  // Start of sensor range
      #define R_V1_START_DEGREES_L  (SDO + 19)  //
      #define R_V1_STEP_DEGREES_H   (SDO + 20)  // Size of sensor range
      #define R_V1_STEP_DEGREES_L   (SDO + 21)  //
                                                // + 22 spare
                                                // + 23 spare
      #define R_V2_MODE             (SDO + 24)  // Virtual sensor 2 Mode (only first register is defined, see R_V0 for other registers!)
      #define R_V3_MODE             (SDO + 32)  // Virtual sensor 3 Mode (only first register is defined, see R_V0 for other registers!)
      #define R_V4_MODE             (SDO + 40)  // Virtual sensor 4 Mode (only first register is defined, see R_V0 for other registers!)
      #define R_V5_MODE             (SDO + 48)  // Virtual sensor 5 Mode (only first register is defined, see R_V0 for other registers!)
      #define R_V6_MODE             (SDO + 56)  // Virtual sensor 6 Mode (only first register is defined, see R_V0 for other registers!)
      #define R_V7_MODE             (SDO + 64)  // Virtual sensor 7 Mode (only first register is defined, see R_V0 for other registers!)
      // *** only 16 byte-pairs can be read at once! *** //
      // 32 bytes of Array data
      #define SDOA (SDO + 72)
      #define R_A0_DISTANCE_H       (SDOA +  0) // Segment distance
      #define R_A1_DISTANCE_H       (SDOA +  2) // Segment distance
      #define R_A2_DISTANCE_H       (SDOA +  4) // Segment distance
      #define R_A3_DISTANCE_H       (SDOA +  6) // Segment distance
      #define R_A4_DISTANCE_H       (SDOA +  8) // Segment distance
      #define R_A5_DISTANCE_H       (SDOA + 10) // Segment distance
      #define R_A6_DISTANCE_H       (SDOA + 12) // Segment distance
      #define R_A7_DISTANCE_H       (SDOA + 14) // Segment distance
      #define R_A8_DISTANCE_H       (SDOA + 16) // Segment distance
      #define R_A9_DISTANCE_H       (SDOA + 18) // Segment distance
      #define R_AA_DISTANCE_H       (SDOA + 20) // Segment distance
      #define R_AB_DISTANCE_H       (SDOA + 22) // Segment distance
      #define R_AC_DISTANCE_H       (SDOA + 24) // Segment distance
      #define R_AD_DISTANCE_H       (SDOA + 26) // Segment distance
      #define R_AE_DISTANCE_H       (SDOA + 28) // Segment distance
      #define R_AF_DISTANCE_H       (SDOA + 30) // Segment distance
      // *** only 16 byte-pairs can be read at once! *** //
      // 32 bytes of Virtual Sensor data
      #define SDOV (SDO + 136)
      #define R_V0_DISTANCE_H       (SDOV +  0)    // Sensor distance
      #define R_V0_HRDEGREES_H      (SDOV +  2)    // Sensor angle (degrees * 32)
      #define R_V1_DISTANCE_H       (SDOV +  4)    // Sensor distance
      #define R_V1_HRDEGREES_H      (SDOV +  6)    // Sensor angle (degrees * 32)
      #define R_V2_DISTANCE_H       (SDOV +  8)    // Sensor distance
      #define R_V2_HRDEGREES_H      (SDOV + 10)    // Sensor angle (degrees * 32)
      #define R_V3_DISTANCE_H       (SDOV + 12)    // Sensor distance
      #define R_V3_HRDEGREES_H      (SDOV + 14)    // Sensor angle (degrees * 32)
      #define R_V4_DISTANCE_H       (SDOV + 16)    // Sensor distance
      #define R_V4_HRDEGREES_H      (SDOV + 18)    // Sensor angle (degrees * 32)
      #define R_V5_DISTANCE_H       (SDOV + 20)    // Sensor distance
      #define R_V5_HRDEGREES_H      (SDOV + 22)    // Sensor angle (degrees * 32)
      #define R_V6_DISTANCE_H       (SDOV + 24)    // Sensor distance
      #define R_V6_HRDEGREES_H      (SDOV + 26)    // Sensor angle (degrees * 32)
      #define R_V7_DISTANCE_H       (SDOV + 28)    // Sensor distance
      #define R_V7_HRDEGREES_H      (SDOV + 30)    // Sensor angle (degrees * 32)
      //------------------
      // I2c registers END
      //------------------
// <FROM LIDER PREPOCESS SKETCH/>
#ifdef ROBOTLIB_BASE_H
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
   short SampleRate;    //  4 & 5- samples in last 100 ms
   char free06;         // 6
   char  Cmd;           //  7 - just about always zero....
   char  RotSpeed;      //  8
   char  OffsetX;       //  9 - in 2mm steps
   char  OffsetY;       // 10 - in 2mm steps
   short OffsetAngle;   // 11 & 12 - in degrees
   char  Reverse;       // 13
   char  free14;        // 14
   char  free15;        // 15
   char  CanSamples;    // 16 - min # of samples for CAN detection
   char  CanEdge;       // 17 - min edge size (delta between two samples, in mm) for CAN start/end
   char  CanMin;        // 18 - min angle of can (units tbd)
   char  CanMax;        // 19 - max angle of can (units tbd)
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
      bool Demo()                         { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_CMD,               3           ); };
      bool SetReverse(char TrueFalse)     { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_REVERSE_ROTATION,  TrueFalse   ); };
      bool SetOffsetDegrees(int Degrees)  { return I2cWrite_Byte_Word(LPP_I2C_ADDRESS, R_OFFSET_DEGREES_H,  Degrees     ); };
      bool SetOffsetX(int Mm)             { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_OFFSET_X,          Mm / 2      ); };
      bool SetOffsetY(int Mm)             { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_OFFSET_Y,          Mm / 2      ); };
      bool SetCanSamples(int Count)       { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_CAN_SAMPLES,       Count       ); };
      bool SetCanEdge(int Mm)             { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_CAN_MIN,           Mm          ); };
      bool SetCanMin(int Min)             { return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_CAN_MAX,           Min         ); };

      bool ArraySetup(int StartAngle, int StepAngle, int StepCount);
      bool SensorSetup(int Nr, int StartAngle, int StepAngle);
      bool SensorSetupCan(int Nr, int StartAngle, int StepAngle);

      bool ReadStatus();
      bool ReadArray();
      bool ReadSensors(int Count = 8);

      bool IsRunning();
      void PrintStatus();
      void PrintArray();
      void PrintSensors();

      void ReadPrintSensorCfg(int Nr);

      int I2cDebug; // for non-robotlib Arduino only
  private:
      int EnableMode;   // 0 = begin not called (or no succes), 1 = inactive, 2 = active
      bool _SA_Setup(char StartIx, char Mode, int StartAngle, int StepAngle, int StepCount);
      bool _ReadShorts(char StartIx, char NrOfShorts, lpp_int *Data);

      int ArrayCount;   // set by ArraySetup()
      int SensorCount;  // set by (last) ReadSensors()
} ;

extern TLpp Lpp; // the default one

// --- C++ ---
#ifdef MAIN
//-----------------------------------------------------------------------------
// TLpp::TLpp - constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TLpp::TLpp()
   {

#ifdef ROBOTLIB_BASE_H
  #ifndef RLM_AUTO_CLI_ADD_DISABLE
      CliAddCommands(CliRecords, "Lpp", "lpp");
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
            if (Status.SwRevision != INTERFACE_VERSION) {
               CSerial.printf("Lpp.begin() failed (interface version error - %d, expected %d\n", (int) Status.SwRevision, INTERFACE_VERSION);
               delay(250);
               continue;
            }
            break;
         }
         delay(250);
      }

      if (r == false) {
         CSerial.printf("Lpp.begin() failed (i2c error)\n");
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
         CSerial.printf("Lpp.Start() error: EnableMode 0\n");
         return false;  // begin not called, of failure
      }
      EnableMode = 2;   // active

      return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_CMD, 1);
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

      return I2cWrite_Byte_Byte(LPP_I2C_ADDRESS, R_CMD, 2);
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
      return _SA_Setup(R_A_MODE, 1, StartAngle, StepAngle, ArrayCount);
   }

//-----------------------------------------------------------------------------
// TLpp::SensorSetup - Setup sensor 0...7 for DISTANCE/ANGLE mode
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TLpp::SensorSetup(int Nr, int StartAngle, int StepAngle)
   {
      return _SA_Setup(R_V0_MODE + Nr * 8, 3, StartAngle, StepAngle, 0);
   }

//-----------------------------------------------------------------------------
// TLpp::SensorSetupCan - Setup sensor 0...7 for CAN mode
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TLpp::SensorSetupCan(int Nr, int StartAngle, int StepAngle)
   {
      return _SA_Setup(R_V0_MODE + Nr * 8, 4, StartAngle, StepAngle, 0);
   }

//-----------------------------------------------------------------------------
// TLpp::ReadStatus - read status data from Lpp and store in class struct
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TLpp::ReadStatus()
   { char TxBuffer[1];

      TxBuffer[0] = R_ID;
      bool r = I2cSendReceive(LPP_I2C_ADDRESS, 1, sizeof(Status), (lpp_tx_buffer *)TxBuffer, (lpp_rx_buffer *)&Status);

      // swap bytes of short.
      unsigned int t;
      t = Status.SampleRate;
      Status.SampleRate  = ((t & 0xFF) << 8) |  ((t >> 8) & 0xFF);

      t = Status.OffsetAngle;
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
      return _ReadShorts(R_A0_DISTANCE_H, ArrayCount, (lpp_int *) Array);
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
      return _ReadShorts(R_V0_DISTANCE_H, SensorCount * 2, (lpp_int *) Sensor);
   }

//-----------------------------------------------------------------------------
// TLpp::PrintStatus -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TLpp::PrintStatus()
   {
      CSerial.printf("LidarID: %d, Sw: %d, RotCount: %d, RotTime: %d, SampleRate: %d\n",
            Status.ID, Status.SwRevision, Status.RotationCount, Status.RotationTime, Status.SampleRate);
      CSerial.printf("Cfg RotSpeed: %d, OffsetAngle: %d, Reverse: %d\n",
            Status.RotSpeed, Status.OffsetAngle, Status.Reverse);
   }

//-----------------------------------------------------------------------------
// TLpp::PrintArray -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TLpp::PrintArray()
   {
      if (EnableMode != 2) CSerial.printf("Lpp.PrintArray() warning: EnableMode != 2\n");

      for (int i=0; i<ArrayCount; i++) {
         CSerial.printf("%4d ", Array[i].Distance);
      }
      CSerial.printf("\n");
   }

//-----------------------------------------------------------------------------
// TLpp::PrintSensors -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TLpp::PrintSensors()
   {
      if (EnableMode != 2) CSerial.printf("Lpp.PrintSensors() warning: EnableMode != 2\n");

      for (int i=0; i<SensorCount; i++) {
         CSerial.printf("%d Distance: %d, Degrees: %d (%d)\n", i, Sensor[i].Distance, NormDegrees(Sensor[i].Degrees32 / 32), Sensor[i].Degrees32);
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
         CSerial.printf("Error: sensor # out of range (%d), 0..7 for sensors, -1 for array.\n", Nr);
         return;
      }

      TxBuffer[0] = R_A_MODE + Nr * 8;
      I2cSendReceive(LPP_I2C_ADDRESS, 1, 6, (lpp_tx_buffer *)TxBuffer, (lpp_rx_buffer *)RxBuffer);

      short TmpStart = (((int)RxBuffer[2]) << 8) + RxBuffer[3];
      CSerial.printf("Sensor %d, Mode: %d, Count: %d, Start: %d, Step: %d\n",
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

#ifdef ROBOTLIB_BASE_H
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

#ifndef LppWire
   #define LppWire Wire
#endif

//-----------------------------------------------------------------------------
// I2cSendReceive - Send and/or Receive data to/from i2c slave.
//-----------------------------------------------------------------------------
// return: true on success
//-----------------------------------------------------------------------------
bool I2cSendReceive(byte I2cSlaveAddress, byte TxCount, byte RxCount, const byte *TxBuffer, byte *RxBuffer)
{
   byte r;

   if (Lpp.I2cDebug) {
      CSerial.printf("I2cSendReceive(%d %d %d)\n", I2cSlaveAddress, TxCount, RxCount);
   }

   if (TxCount > 0) {
      if (Lpp.I2cDebug > 1) {
         CSerial.printf("TxBuf:\n");
         HexDump(TxBuffer, TxCount);
      }

      LppWire.beginTransmission(I2cSlaveAddress);
      for (int i=0; i<TxCount; i++) {
         LppWire.write(TxBuffer[i]); //
      }
      r = LppWire.endTransmission();
      if (r != 0) return false;  // error
   }

   if (RxCount > 0) {

      for (int i=0; i<RxCount; i++) RxBuffer[i] = 0;

      LppWire.requestFrom(I2cSlaveAddress, RxCount);    // request byte(s) from slave device

      if (LppWire.available() != RxCount) return false;  // error

      for (int i=0; i<RxCount; i++) {
         RxBuffer[i] = LppWire.read();
      }
      if (Lpp.I2cDebug > 1) {
         CSerial.printf("RxBuf:\n");
         HexDump(RxBuffer, RxCount);
      }
   }
   return true;  // success
}

#ifndef HEXDUMP_DEFINED
#define HEXDUMP_DEFINED

/*=====================================================================
 HexDump :
 ---------------------------------------------------------------------*/
// Parameters:
//    Data   - data to be dumped
//    Length - nr of bytes to be dumped
//    Offset - offset of address (from 0), displayed at the start of each line.
//-----------------------------------------------------------------------------
void HexDump( const void *Data, unsigned int Length, unsigned int Offset)
{
   unsigned char *data    = (unsigned char *)Data    ;

   unsigned int Track1 = 0 ;
   unsigned int Track2 = 0 ;

   for (unsigned int Index=0 ; Index < Length ; Index = Index+16) {
      CSerial.printf( "%04x: ", Offset + Index ) ;

      for (unsigned int j=0; j < 16; j++) {
         if( Track1 < Length ) {
            CSerial.printf("%02x", data[Index+j]);
         } else {
            CSerial.printf("  ");
         }

         CSerial.printf( " " ) ;
         Track1++ ;
      }

      CSerial.printf(" ");
      for (unsigned int j=0 ; j < 16 ; j++) {
         if (Track2 < Length) {
            if (data[ Index+j ] < 32 ) {
               CSerial.printf(".");
            } else {
               if (data[ Index+j ] < 127 ) {
                  CSerial.printf( "%c", data[ Index+j ]);
               } else {
                  CSerial.printf(".");
               }
            }
         } else {
            CSerial.printf( " " ) ;
         }
         Track2++ ;
      }
      CSerial.printf( "\r\n" ) ;
   }
}
#endif // HEXDUMP_DEFINED
#endif // MAIN

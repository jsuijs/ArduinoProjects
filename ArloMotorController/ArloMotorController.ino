// ArloMotorController.ino

#include "project.h"
#include "Commands.h"

//https://forum.arduino.cc/index.php?topic=566042.0
HardwareSerial Serial(USART1);
HardwareSerial Serial2(USART2);

// Function prototypes (might be superfluous for Arduino...)
void MyCommands(int Number[]);
void MsgCommands(int Param[]);

// Command interpreters
TCommand Command(MyCommands);
TCommand CmdMessages(MsgCommands);

// global parameters (set by commands/messages, used by loop)
int CfgLoopTime      = 200;   // millis
int CfgTimeOut       =  15;   // loop times
int DriveMode        =   0;   // 0 = off, 1 = pwm, 2 = speed
int TimeOutCounter   =   0;
int PwmL             =   0;
int PwmR             =   0;

//-----------------------------------------------------------------------------
// setup -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setup()
{
   // initialize the digital pin as an output.
   pinMode(Pin_Led, OUTPUT);

   Serial.begin(115200);   // Console
   Serial2.begin(115200);  // interface naar OpenCR.

   EncodersInit();
   MotorsInit();

   delay(100);  // wait for Arduino IDE to re-enable the terminal after programming...

   printf("(RxBuf: %d, TxBuf: %d).\n", SERIAL_RX_BUFFER_SIZE, SERIAL_TX_BUFFER_SIZE);

#ifdef USE_PID
   printf("PID enabled.\n");
#endif

   printf("ArloMotorController V0.07 ready.\n");
}

//-----------------------------------------------------------------------------
// loop -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void loop()
{  static int NextTakt;
   static int EncoderL, EncoderR;
   static bool LedStatus;

   int ms = millis();
   if ((ms - NextTakt) > 0) {
      NextTakt = ms + CfgLoopTime;  // zet tijd voor volgende interval

      //-----------------------------------------------------------------------
      // Timeout safeguard: stop robot when no comms is received
      //-----------------------------------------------------------------------
      if (TimeOutCounter > 0) {
         TimeOutCounter --;
      } else {
         // we are idle
         if (TimeOutCounter == 0) {
            printf("Timeout safeguard: motors stopped.\n");
         }
         DriveMode      = 0;
         TimeOutCounter = -1;  // we're stopped

#ifdef USE_PID
         // reset PIDs
         PidL.SetMode(MANUAL);
         PidR.SetMode(MANUAL);
         PidL_In = PidL_Out = 0;
         PidR_In = PidR_Out = 0;
         PidL.SetMode(AUTOMATIC);
         PidR.SetMode(AUTOMATIC);
#endif
      }

      LedStatus = !LedStatus;
      digitalWrite(Pin_Led, LedStatus);

      //-----------------------------------------------------------------------
      // Encoder stuff
      //-----------------------------------------------------------------------
      int DeltaEncL, DeltaEncR;
      EncodersRead (DeltaEncL, DeltaEncR);

      EncoderL += DeltaEncL;
      EncoderR += DeltaEncR;

      SendMessage("ENCODER", EncoderL, EncoderR);
      SendMessage("SPEED", EncoderSpeedL, EncoderSpeedR);

      if (DeltaEncL || DeltaEncR) {
         printf("EncoderDelta %d %d, Speed: %d %d\n", DeltaEncL*50, DeltaEncR*50, EncoderSpeedL, EncoderSpeedR);
      }

      //-----------------------------------------------------------------------
      // Drive stuff
      //-----------------------------------------------------------------------
      switch (DriveMode) {
         case 0 : {  // Idle
            Motors(0, 0);
         }
         break;

         case 1 : {  // PWM
            Motors(PwmL, PwmR);
         }
         break;

#ifdef USE_PID
         case 2 : {  // Speed, drive motors via PID
            PidL_In = EncoderSpeedL;    // ticks/sec
            PidR_In = EncoderSpeedR;

            PidL.Compute();
            PidR.Compute();

            printf("Pid In: %d %d, sp: %d %d, out: %d %d\n",
               (int) PidL_In, (int) PidR_In,
               (int) PidL_Sp, (int) PidR_Sp,
               (int) PidL_Out, (int) PidR_Out);


            Motors(PidL_Out, PidR_Out);
//            Motors(PidL_Out + PidL_Sp * 1.1, PidR_Out + PidR_Sp * 1.1); // include feed forward
         }
         break;
#endif
      }
   }
   Command.Takt(Serial);  // Console command interpreter
   MessageReceiveTakt();  // Received messages interpreter
}

//-----------------------------------------------------------------------------
// MessageReceiveTakt - Receive & processes messages from Serial2.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageReceiveTakt()
{  static bool ReceivingMsg = false;

   while (Serial2.available() > 0) {
      int ch = Serial2.read();

//      printf("aa %d %d\n", ch, ReceivingMsg);

      if (ch == 0xC1) {
         // Start flag
         CmdMessages.Clear();
         ReceivingMsg = true;
         continue;
      }

      if (ch == 0xC0) {
         // End flag
         ReceivingMsg = false;

         int r = CmdMessages.GetLine('\r');
         if (r == 0) {
            printf("Error 20200506-1\n");   // msg should be done here.
            continue;  // still reading line
         }

         if (r < 0) {
            printf("MSG Cmd parse err %d\n", r);
            CmdMessages.Clear();
            continue;
         }
         //CmdMessages.Print();
         CmdMessages.Execute(1);  // Verbose level 1: only errors.
         CmdMessages.Clear();
      }

      if (ReceivingMsg) {
         int r = CmdMessages.GetLine(ch);
         if (r != 0) {
            printf("Error 20200506-2 (%d)\n", r);   // msg shouldn't be done here.
            ReceivingMsg = false;
            continue;
         }
      }
   }
}

//-----------------------------------------------------------------------------
// MsgCommands - called to process received messages
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MsgCommands(int Param[])
{
   if (CmdMessages.Match("MOTORS", 2)) {
      PwmL           = Param[0];
      PwmR           = Param[1];
      DriveMode      = 1; // PWM
      TimeOutCounter = CfgTimeOut;
   }

   if (CmdMessages.Match("TIMING", 2)) {
      CfgLoopTime = Param[0];    // millis
      CfgTimeOut  = Param[1];    // loop times
   }

#ifdef USE_PID
   if (CmdMessages.Match("SPEED", 2)) {
      PidL_Sp        = Param[0];
      PidR_Sp        = Param[1];
      DriveMode      = 2; // Speed
      TimeOutCounter = CfgTimeOut;
   }
#endif
}

//-----------------------------------------------------------------------------
// SendEncoderMessage -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SendMessage(const char *MsgType, int EncL, int EncR)
{
   Serial2.print((char) 0xC1);
   Serial2.print(MsgType);
   Serial2.print(" ");
   Serial2.print(EncL);
   Serial2.print(" ");
   Serial2.print(EncR);
   Serial2.println((char) 0xC0);
}

//-----------------------------------------------------------------------------
// MyCommands - used / called by Command class.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MyCommands(int Param[])
{
   if (Command.Match("?", 0)) {
      printf("ArloMotorController - see source for help on commands ;)\n");
   }

   if (Command.Match("motors", 2)) {
      PwmL           = Param[0];
      PwmR           = Param[1];
      DriveMode      = 1; // PWM
      TimeOutCounter = CfgTimeOut;
   }

   if (Command.Match("encoders", 0)) {
      int Left, Right;
      EncodersRead (Left, Right);
      printf("Encoders: %d %d\n", Left, Right);
   }

   if (Command.Match("looptime", 1)) {
      CfgLoopTime = Param[0];    // millis
   }

   if (Command.Match("timeout", 1)) {
      CfgTimeOut  = Param[0];    // loop times
   }

#ifdef USE_PID
   if (Command.Match("speed", 2)) {
      PidL_Sp        = Param[0];
      PidR_Sp        = Param[1];
      DriveMode      = 2; // Speed
      TimeOutCounter = CfgTimeOut;
   }

   if (Command.Match("tunings", 3)) {
      // set tunings, same for both motors.
      // Note: scaled by 100 since commands only support int's.
      PidL.SetTunings(Param[0] / 100.0, Param[1] / 100.0, Param[2] / 100.0);
      PidR.SetTunings(Param[0] / 100.0, Param[1] / 100.0, Param[2] / 100.0);
      printf("Tunings: %f %f %f\n", PidL.GetKp(),  PidL.GetKi(),  PidL.GetKd());
   }

   if (Command.Match("tunings", 0)) {
      // print tunings
      printf("Tunings: %f %f %f\n", PidL.GetKp(),  PidL.GetKi(),  PidL.GetKd());
   }
#endif
}

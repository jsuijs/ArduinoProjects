// ArloMotorController.ino

#include "project.h"
#include "commands.h"

//https://forum.arduino.cc/index.php?topic=566042.0
HardwareSerial Serial(USART1);
HardwareSerial Serial2(USART2);

void MyCommands(int Number[]);
TCommand Command(MyCommands);

void MsgCommands(int Param[]);
TCommand CmdMessages(MsgCommands);

int CfgLoopTime      = 200;   // millis
int CfgTimeOut       = 15;    // loop times

//-----------------------------------------------------------------------------
// setup -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setup()
{
   // initialize the digital pin as an output.
   pinMode(Pin_Led, OUTPUT);

   // LET OP: het is blijkbaar van belang ook de 2e poort te openen aan het begin
   // van het programma. Anders lijkt serial2 toch usart1 te worden....
   Serial.begin(115200);   // Console
   Serial2.begin(115200);  // interface naar OpenCR.

   //  Serial.println("Serial _ ready.");
   Serial2.println("Serial2 ready.");

   EncodersInit();
   MotorsInit();

   PidTakt(0, 0); // init

   printf("ArloMotorController ready.\n");
}

int DriveMode        = 0;   // 1 = pwm, 2 = speed
int TimeOutCounter   = 0;
int PwmL, PwmR;

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

      // Safeguard: stop robot when no comms is received.
      if (TimeOutCounter < 0) {
         // we are idle
         DriveMode      = 0;
         TimeOutCounter = 0;  // keep it at 0 or -1
      }
      TimeOutCounter --;

      LedStatus = !LedStatus;
      digitalWrite(Pin_Led, LedStatus);

      //--------------
      // Encoder stuff
      //--------------
      int DeltaEncL, DeltaEncR;
      EncodersRead (DeltaEncL, DeltaEncR);
      PidL_In = DeltaEncL;
      PidR_In = DeltaEncR;

      EncoderL += DeltaEncL;
      EncoderR += DeltaEncR;

      // Output ENCODERS message
      Serial.print(0xC1);
      Serial.print("ENCODERS ");
      Serial.print(EncoderL);
      Serial.print(" ");
      Serial.print(EncoderR);
      Serial.println(0xC0);

      //------------
      // Drive stuff
      //------------
      switch (DriveMode) {
         case 0 : {  // Idle
            Motors(0, 0);
         }
         break;

         case 1 : {  // PWM
            Motors(PwmL, PwmR);
         }
         break;

         case 2 : {  // Speed, drive motors via PID
            PidTakt(0, 0);
         }
         break;
      }
   }

   Command.Takt(Serial);  // Console command interpreter
   MessageReceiveTakt();  // Received messages interpreter
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

   if (Command.Match("speed", 2)) {
      PidL_Sp        = Param[0];
      PidR_Sp        = Param[1];
      DriveMode      = 2; // Speed
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
}

//-----------------------------------------------------------------------------
// MessageReceiveTakt - Receive & processes messages from Serial2.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageReceiveTakt()
{  bool ReceivingMsg = false;

   while (Serial2.available() > 0) {
      int ch = Serial2.read();

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

   if (Command.Match("MOTORS", 2)) {
      PwmL           = Param[0];
      PwmR           = Param[1];
      DriveMode      = 1; // PWM
      TimeOutCounter = CfgTimeOut;
   }

   if (Command.Match("SPEED", 2)) {
      PidL_Sp        = Param[0];
      PidR_Sp        = Param[1];
      DriveMode      = 2; // Speed
      TimeOutCounter = CfgTimeOut;
   }

   if (Command.Match("TIMING", 2)) {
      CfgLoopTime = Param[0];    // millis
      CfgTimeOut  = Param[1];    // loop times
   }
}
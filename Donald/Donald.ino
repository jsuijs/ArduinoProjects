//-----------------------------------------------------------------------------
// UbtServoDemo.ino - demo program for UbTech servo control
//-----------------------------------------------------------------------------
#define MAIN
#include "Project.h"

HardwareSerial Serial2 (PA3, PA2);  // console serial

//---------------------------------------------------------------------------------------
// RC5 stuff start
#include "Libs/RC5.h"

int Rc5Data;  // Set on receive, feel free to set to zero when done.
int IR_PIN = PB10;
//int RC5_INTERRUPT = 0;
RC5 rc5(IR_PIN);

void Rc5Isr()
{ static unsigned int   PrevMessage;
  unsigned int Message;
  if (rc5.read(&Message)) {
    if (Message != PrevMessage) {
      Rc5Data     = Message;
      PrevMessage = Message;
    }
  }
}
// Rc5 stuff done (but do not forget to attach Rc5Isr() to IrPin).
//---------------------------------------------------------------------------------------
#include "Libs/RcDispatch.cpp"

TFlags Flags(32);

#include "UbTechSerial.h"  // UBT serial port (stm32f1 specific, hardcoded to Serial1)
#include "UbTechServo.h"   // servo class
uKitServo UbtServo;

#include "Biped.h"

// command-parser
void Execute();
TCommand  Command(Execute);

int InSequence = 0;

//---------------------------------------------------------------------------------------
// setup -
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void setup() {
   // start serial
   CSerial.begin(115200);
   CSerial.printf("Start\n");

   // Link PinChange interrupt to RC5 reader.
   attachInterrupt(IR_PIN, Rc5Isr, CHANGE);

   UbtSetup();
   UbtServo.getServoId(9);    // first message always fails, so have that failure now
                              // (works when servo # exists and when it doesn't...).

   pinMode(PB1, OUTPUT);      //Led on Maple-Mini

   CSerial.printf("Startup done.\n");
}

//---------------------------------------------------------------------------------------
// loop -
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void loop() {
   static int PrevMs;

   // once per milisecond
   int ms = millis();
   if (ms != PrevMs) {
      PrevMs = ms;
      BlinkTakt();
   }

   ActionEngine.Takt(InSequence);

   Command.Takt(CSerial);  // Console command interpreter
   RcDispatch(Rc5Data);

   int ch = PfKeyGet();
   if (ch) {
      // knop ingedrukt

      CSerial.printf("Key: %d\n", ch);
//      if (ch == -1) {
//         Program.Reset();           // reset, stop lopend programma / programma 'stilstaan'.
//      } else {
//         if (Program.State == 0) {  // andere pfkeys werken alleen als we stil staan
//            Program.State = ch;
//         }
//      }
   }
}

//-----------------------------------------------------------------------------
// Execute - execute commando
//-----------------------------------------------------------------------------
// Called via CmdTakt() when a command is received from the serial port.
//-----------------------------------------------------------------------------
void Execute(int Param[])
{
   if (Command.Match("?",                 0)) Command.Help("ArduinoPlanck command parser.");

   if (Command.Match("Flag",              1)) CSerial.printf("Flag %d is %d\n", Param[0], Flags.IsSet(Param[0]));
   if (Command.Match("Flag",              2)) Flags.Set(Param[0], Param[1]);
   if (Command.Match("FlagDump",          0)) Flags.Dump();
   if (Command.Match("PfKey",             1)) PfKeySet(Param[0]);

   // Servo commands
   if (Command.Match("ServoSetTurn",      3)) UbtServo.setServoTurn(Param[0], Param[1], Param[3]);  // id dir speed
   if (Command.Match("ServoSetStif",      2)) UbtServo.setServoStiffness(Param[0], Param[1]);
   if (Command.Match("ServoSetAngle",     2)) UbtServo.setServoAngle(Param[0], Param[1], 200);
   if (Command.Match("ServoSetAngle",     3)) UbtServo.setServoAngle(Param[0], Param[1], Param[2]);
   if (Command.Match("ServoSetStop",      1)) UbtServo.setServoStop(Param[0]);
   if (Command.Match("Scan",              0)) UbtServo.Scan(); // scan for servo's & print result
   if (Command.Match("ServoGetId",        1)) CSerial.printf("R: %d\n", (int)UbtServo.getServoId(Param[0]));
   if (Command.Match("ServoSetId",        2)) CSerial.printf("R: %d\n", (int)UbtServo.setServoId(Param[0], Param[1]));
   if (Command.Match("ServoReadAngle",    1)) printf("Degrees: %d\n", UbtServo.readServoAngleNPD(Param[0]));
   if (Command.Match("ServoReadAnglePD",  1)) printf("Degrees: %d\n", UbtServo.readServoAnglePD(Param[0]));

   // Biped-specific commands.
   if (Command.Match("BSet",              6)) BipedSet(Param);
   if (Command.Match("BRead",             0)) BipedRead();
   if (Command.Match("Pose",              1)) ActionEngine.TestPose(Param[0]);
   if (Command.Match("Sequence",          1)) InSequence = Param[0];
}

//-----------------------------------------------------------------------------
// _write - link printf to CSerial
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern "C" int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++) {
       CSerial.print(*ptr++);
    }
    return len;
}

//-----------------------------------------------------------------------------
// BlinkTakt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BlinkTakt()
{  static int Count;

   if (Count > 100) {
      digitalWrite(PB1, ! digitalRead(PB1));
      Count = 0;
   }
   Count ++;
}

//-----------------------------------------------------------------------------
// UbtServoDemo.ino - demo program for UbTech servo control
//-----------------------------------------------------------------------------
#define MAIN
#include "Project.h"

HardwareSerial Serial2 (PA3, PA2);  // console serial

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

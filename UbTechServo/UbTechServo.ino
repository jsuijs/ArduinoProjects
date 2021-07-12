//-----------------------------------------------------------------------------
// LiMaPlanck.ino
//-----------------------------------------------------------------------------

#define MAIN
#include "RobotSettings.h"          // project-instellingen
#include "Libs/MyRobot.h"           // super-include
#include "Project.h"

void Execute();   // prototype

// global instances
TCommand       Command(Execute);

HardwareSerial Serial2 (PA3, PA2);  // console serial

#include "UbTechSerial.h"           // UBT serial port (hardcoded to Serial1)
#include "uKitServo.h"              // servo class
uKitServo UbtServo;

//                   SDA   SCL
TwoWire        Wire2(PB11, PB10);
TFlags         Flags(32);

// Create buzzer instance & call it each ms from SYSTICK
TBuzzer Buzzer(BUZZER_PIN);
void HAL_SYSTICK_Callback(void) { Buzzer.Takt(); }

//---------------------------------------------------------------------------------------
// setup -
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void setup() {
   // start serial
   CSerial.begin(115200);
   CSerial.printf("Starten\n");

   UbtSetup();

   pinMode(PB1, OUTPUT);    //Led op Maple-Mini

   I2cClearBus(PB11, PB10); // SDA, SCL
   LppWire.begin();

   Flags.Set(1, true);     // Drive FirstCall - all movements (sub takt)
   //Flags.Set(2, true);     // 2 Driver.SpeedLRTakt
   //Flags.Set(3, true);     // 3 Driver.RotateRelTakt
   //Flags.Set(4, true);     // 4 Driver.SpeedHeadingTakt
   //Flags.Set(5, true);     // 5 Driver.ArcRelTakt
   //Flags.Set(6, true);     // 6 Driver.XYTakt
   Flags.Set(7, true);     // 7 Driver.SpeedRotationTakt

   Flags.Set(20, true);     // Position print (each update)

   //Flags.Set(9, true);   // Lpp array dump
   Flags.Set(10, true);    // ProgrammaTakt programma-keuze
   Flags.Set(11, true);    // ProgrammaTakt Missie-takt

   Buzzer.Beep(30, 2);
   CSerial.printf("Opstarten gereed.\n");
}

//---------------------------------------------------------------------------------------
// loop -
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void loop() {
   static int NextMainTakt;
   static int NextSecTakt;
   static int PrevMs;

   // eens per miliseconde
   int ms = millis();
   if (ms != PrevMs) {  // miliseconde takt
      PrevMs = ms;  // bewaar huidige tijd
      BlinkTakt();
   }
   // Main takt interval
   ms = millis();
   if ((ms - NextMainTakt) > 0) {
      NextMainTakt = ms + MAIN_TAKT_INTERVAL;  // zet tijd voor volgende interval
      // hier de periodieke acties voor deze interval

   }

   // Seconde interval
   ms = millis();
   if ((ms - NextSecTakt) > 0) {
      NextSecTakt = ms + 1000;  // zet tijd voor volgende interval
      // hier de periodieke acties voor deze interval

   }

   Command.Takt(CSerial);  // Console command interpreter
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

   if (Command.Match("ServoReadAngle",    1)) printf("Hoek: %d\n", UbtServo.readServoAngleNPD(Param[0]));
   if (Command.Match("ServoReadAnglePD",  1)) printf("Hoek: %d\n", UbtServo.readServoAnglePD(Param[0]));
}


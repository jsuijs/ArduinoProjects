//-----------------------------------------------------------------------------
// Planck.ino
//-----------------------------------------------------------------------------

#define MAIN
#include "RobotSettings.h"          // project-instellingen
#include "Libs/MyRobot.h"           // super-include
#include "Libs/Motors_TB6612.cpp"   // Kies je motor-sturing

// include code

void Execute();   // prototype

// global instances
TPosition      Position;
TDrive         Driver;
TCommand       Command(Execute);
TLpp           Lpp;
HardwareSerial Serial2 (PA3, PA2);
TwoWire        Wire2(PB11, PB10);
TFlags         Flags(32);

int Lijn;  // 0..7, 3 bits. 0 = wit, 7 = zwart, 1 = links, 2 = midden, 4 = rechts

//---------------------------------------------------------------------------------------
// RC5 stuff start
#include "Libs/RC5.h"

int Rc5Data;  // Set on receive, feel free to set to zero when done.
int IR_PIN = PB4;
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

void setup() {
   // start serial
   CSerial.begin(115200);
   CSerial.printf("Starten\n");

   Position.init();  // delayed constructor
   Driver.init();    // delayed constructor

   SetupMotors();
   InitStmEncoders();

   pinMode(PB1, OUTPUT);    //Led op Maple-Mini

   // Link PinChange interrupt to RC5 reader.
   attachInterrupt(IR_PIN, Rc5Isr, CHANGE);

   LppWire.begin();
   if (Lpp.begin()) {

      Lpp.SetOffsetDegrees(180);       //180 Align lidar with robotlib coordinate system
      Lpp.SetReverse(0);            // 1

      Lpp.ArraySetup(70, 20, 11);      // Setup array with 11 segments of 20 degrees
      Lpp.SensorSetup(0, -15, 30);    // Setup Sensor 0 to detect achterwaards objects (-15 + 30 = +15 graden)
      Lpp.SensorSetup(1, 90, 180);    // Sensor 1, vanaf 90 graden, (+90 + 180 = 270 graden)
      Lpp.SensorSetup(2, 135, 90);    // Sensor 2, vanaf 135 graden, segment van 90 graden
      Lpp.SensorSetup(3, 70, 40);     // Sensor 3, vanaf 70 graden, segment van 40 graden LockDown 5=8-Slalom
      Lpp.SensorSetup(4, 110, 40);    // Sensor 4, vanaf 110 graden, segment van 40 graden
      Lpp.SensorSetup(5, 150, 60);    // Sensor 4, vanaf 150 graden, segment van 60 graden
      Lpp.SensorSetup(6, 210, 40);    // Sensor 4, vanaf 210 graden, segment van 40 graden
      Lpp.SensorSetup(7, 250, 40);    // Sensor 7, vanaf 250 graden, segment van 40 graden

      // Lees en print status (ter informatie)
      Lpp.ReadStatus();
      Lpp.PrintStatus();
   } else {
      CSerial.printf("LPP I2C error.\n");
   }

   Flags.Set(10, true);
   Flags.Set(11, true);

   CSerial.printf("Opstarten gereed.\n");
}

void loop() {
   static int NextMainTakt;
   static int NextSecTakt;
   static int PrevMs;

   RcDispatch(Rc5Data);

   // eens per miliseconde
   int ms = millis();
   if (ms != PrevMs) {  // miliseconde takt
      PrevMs = ms;  // bewaar huidige tijd
      BlinkTakt();
      Lijn = 7 - (digitalRead(5) * 1 + digitalRead(6) * 2 + digitalRead(7) * 4);
   }
   // Main takt interval
   ms = millis();
   if ((ms - NextMainTakt) > 0) {
      NextMainTakt = ms + MAIN_TAKT_INTERVAL;  // zet tijd voor volgende interval
      // hier de periodieke acties voor deze interval

      Lpp.ReadSensors();// lees lidar data
      Position.Takt();  // Lees & verwerk encoder data
      ProgrammaTakt();  // Voer (stapje van) geselecteerde programma uit
      Driver.Takt();    // stuur motoren aan
   }

   // Seconde interval
   ms = millis();
   if ((ms - NextSecTakt) > 0) {
      NextSecTakt = ms + 1000;  // zet tijd voor volgende interval
      // hier de periodieke acties voor deze interval

      if (Flags.IsSet(1)) {
         Position.Print();
      }

      //int Batterij = analogRead(BATTERIJ_PIN);
      //int Spanning = (int) (145L * Batterij / 960);  // 14.8 volt geeft waarde 964
      //CSerial.printf("Batterij: %d (V * 10) (%d)\n", Spanning, Batterij);
      //CSerial.printf("Lijn: %d (%d %d %d)\n", Lijn, digitalRead(5), digitalRead(6), digitalRead(7));
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
// Execute - voer commando uit
//-----------------------------------------------------------------------------
// wordt via CmdTakt() aangeroepen als een commando is ontvangen
// via de serial port.
//-----------------------------------------------------------------------------
void Execute(int Param[])
{
   if (Command.Match("?",              0)) Command.Help("ArduinoPlanck command parser.");

   // Drive commands
   if (Command.Match("DrivePwm",       2)) Driver.Pwm(Param[0], Param[1]);
   if (Command.Match("DriveLR",        2)) Driver.SpeedLR(Param[0], Param[1]);
   if (Command.Match("DriveSH",        2)) Driver.SpeedHeading(Param[0], Param[1]);
   if (Command.Match("DriveXY",        4)) Driver.XY(Param[0], Param[1], Param[2], Param[3]);
   if (Command.Match("DriveRotate",    1)) Driver.Rotate(Param[0]);
   if (Command.Match("DriveRotateRel", 1)) Driver.RotateRel(Param[0]);
   if (Command.Match("DriveArc",       4)) Driver.Arc(Param[0], Param[1], Param[2], Param[3]);
   if (Command.Match("DriveStop",      0)) Driver.Stop();

   if (Command.Match("LppStatus",      0)) { Lpp.ReadStatus(); Lpp.PrintStatus(); }
   if (Command.Match("LppStart",       0)) Lpp.Start();
   if (Command.Match("LppStop",        0)) Lpp.Stop();

   if (Command.Match("PfKey",          1)) PfKeySet(Param[0]);
   if (Command.Match("Position",       0)) Position.Print();
   if (Command.Match("PositionReset",  0)) Position.Reset();

   if (Command.Match("Flag",           1)) CSerial.printf("Flag %d is %d\n", Param[0], Flags.IsSet(Param[0]));
   if (Command.Match("Flag",           2)) Flags.Set(Param[0], Param[1]);
   if (Command.Match("FlagDump",       0)) Flags.Dump();
}
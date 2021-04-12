//-----------------------------------------------------------------------------
// Planck.ino
//-----------------------------------------------------------------------------

// Robot-specifieke parameters
#define RC5_CPP
#include "MyRobot.h"
#include "Libs\Utilities.cpp"

// include Position class & instantieer
TPosition Position;

// include Drive class & instantieer
TDrive Driver;
bool SecondLoggingOn = true;

// Setup command interpreter
void Execute();   // prototype
#include "Libs/Commands.h"
TCommand Command(Execute);

// Setup i2c & Lidar Preprocessor
#include <Wire.h>
TwoWire Wire2(PB11, PB10);
#define LppWire Wire2

#include "Libs/LppMaster.h"
TLpp Lpp;

int Lijn;  // 0..7, 3 bits. 0 = wit, 7 = zwart, 1 = links, 2 = midden, 4 = rechts

HardwareSerial Serial2 (PA3, PA2);

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
//   Serial.println("Starten....\n");
   CSerial.printf("Starten 3\n");

   Position.init();  // delayed constructor
   Driver.init();    // delayed constructor

   SetupMotors();
   InitStmEncoders();

   pinMode(PB1, OUTPUT);    //Led op Maple-Mini

   // Link PinChange interrupt to RC5 reader.
   attachInterrupt(PB4, Rc5Isr, CHANGE);

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
      Position.Takt(); // Lees & verwerk encoder data
      ProgrammaTakt(); // Voer (stapje van) geselecteerde programma uit
      Driver.Takt();   // stuur motoren aan
   }

   // Seconde interval
   ms = millis();
   if ((ms - NextSecTakt) > 0) {
      NextSecTakt = ms + 1000;  // zet tijd voor volgende interval
      // hier de periodieke acties voor deze interval

      if (SecondLoggingOn) {
         // Optional logging, toggle with LIST button.
         //
         // CSerial.printf("Encoder L/R TPrev %d/%d TAct: %d/%d, Count: %d/%d\n",
         //       EncoderLPeriode, EncoderRPeriode, EncoderL_LopendePeriode, EncoderR_LopendePeriode, EncoderLTeller, EncoderRTeller);
         Position.Print();
         //int Batterij = analogRead(BATTERIJ_PIN);
         //int Spanning = (int) (145L * Batterij / 960);  // 14.8 volt geeft waarde 964
         //CSerial.printf("Batterij: %d (V * 10) (%d)\n", Spanning, Batterij);
//         CSerial.printf("Lijn: %d (%d %d %d)\n", Lijn, digitalRead(5), digitalRead(6), digitalRead(7));
      }
   }

   Command.Takt(CSerial);  // Console command interpreter
}


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
   if (Command.Match("?",              0)) CSerial.printf("ArduinoPlanck command parser.\n");

   // Drive commands
   if (Command.Match("drivepwm",       2)) Driver.Pwm(Param[0], Param[1]);
   if (Command.Match("drivelr",        2)) Driver.SpeedLR(Param[0], Param[1]);
   if (Command.Match("drivesh",        2)) Driver.SpeedHeading(Param[0], Param[1]);
   if (Command.Match("drivexy",        4)) Driver.XY(Param[0], Param[1], Param[2], Param[3]);
   if (Command.Match("driverotate",    1)) Driver.Rotate(Param[0]);
   if (Command.Match("drivearc",       4)) Driver.Arc(Param[0], Param[1], Param[2], Param[3]);
   if (Command.Match("drivestop",      0)) Driver.Stop();

   if (Command.Match("lppstatus",      0)) { Lpp.ReadStatus(); Lpp.PrintStatus(); }
   if (Command.Match("lppstart",       0)) Lpp.Start();
   if (Command.Match("lppstop",        0)) Lpp.Stop();

   if (Command.Match("pfkey",          1)) PfKeySet(Param[0]);

}
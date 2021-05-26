//-----------------------------------------------------------------------------
// LiMaPlanck.ino
//-----------------------------------------------------------------------------

#define MAIN
#include "RobotSettings.h"          // project-instellingen
#include "Libs/MyRobot.h"           // super-include
#include "Project.h"

// include code

void Execute();   // prototype

// global instances
TCommand       Command(Execute);

HardwareSerial Serial2 (PA3, PA2);

#include"uKitServo.h"
uKitServo UbtServo;
#include "UbTech.h"

//                   SDA   SCL
TwoWire        Wire2(PB11, PB10);
TFlags         Flags(32);

Servo myservo;  // create servo object to control a servo

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

   // Link PinChange interrupt to RC5 reader.
   attachInterrupt(IR_PIN, Rc5Isr, CHANGE);

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

   myservo.attach(PB5);  // attaches the servo on pin 9 to the servo object
}

//---------------------------------------------------------------------------------------
// loop -
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
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
   if (Command.Match("?",              0)) Command.Help("ArduinoPlanck command parser.");

   if (Command.Match("PfKey",          1)) PfKeySet(Param[0]);

   if (Command.Match("Flag",           1)) CSerial.printf("Flag %d is %d\n", Param[0], Flags.IsSet(Param[0]));
   if (Command.Match("Flag",           2)) Flags.Set(Param[0], Param[1]);
   if (Command.Match("FlagDump",       0)) Flags.Dump();

   if (Command.Match("Servo",          1)) myservo.write(Param[0]);
}


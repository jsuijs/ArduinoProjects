//-----------------------------------------------------------------------------
// Planck.ino
//-----------------------------------------------------------------------------

// Dummy functie, zodat Arduino de vereiste prototypes plaatst.
void DummyFunctie() { }

// Robot-specifieke parameters
#define RC5_CPP
#include "MyRobot.h"

// include Position class & instantieer
TPosition Position;

// include Drive class & instantieer
TDrive Driver;
bool SecondLoggingOn = true;


int Lijn;  // 0..7, 3 bits. 0 = wit, 7 = zwart, 1 = links, 2 = midden, 4 = rechts

HardwareSerial Serial2 (PA3, PA2);

//---------------------------------------------------------------------------------------
// RC5 stuff start
#include "RC5.h"

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

   // Link PinChange interrupt to RC5 reader.
   attachInterrupt(PB4, Rc5Isr, CHANGE);

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
}


void BlinkTakt()
{  static int Count;


   if (Count > 300) {
      digitalWrite(7, ! digitalRead(7));
      Count = 0;
   }
   Count ++;
}

//---------------------------------------------------------------------------------------

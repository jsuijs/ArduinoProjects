//-----------------------------------------------------------------------------
// Planck.ino
//-----------------------------------------------------------------------------

// Dummy functie, zodat Arduino de vereiste prototypes plaatst.
void DummyFunctie() { } 

// Robot-specifieke parameters
#include "RobotSettings.h"

// Hier de algemene definities
#define MAIN_TAKT_RATE     (1000 / MAIN_TAKT_INTERVAL)   // Hz
#define WIEL_BASIS         ((ODO_TICK_TO_METRIC * 917L) / ODO_HEADING)
#define RAD2GRAD(x) ((float)(x) * 57.2957795)   // uitkomst is float, deze kan evt zonder verlies geschaald worden naar hogere resulotie
#define ACT_SPEED_MM_SEC(ActSpeed) ((ActSpeed * (long)ODO_TICK_TO_METRIC)) / (4 * MAIN_TAKT_INTERVAL);
#define ABS(x) ( (x>=0) ? x : -x )

// include Position class & instantieer
#include "Position.h"
TPosition Position;

// include Drive class & instantieer
#include "Drive.h"
TDrive Driver;
bool SecondLoggingOn = true;

int SharpVoor, SharpRechts;    // sharp meting - afstand in mm
long SharpVoor_Gemiddelde, SharpRechts_Gemiddelde;


//---------------------------------------------------------------------------------------
// RC5 stuff start
//#include <PinChangeInt.h>
#include "RC5.h"

int Rc5Data;  // Set on receive, feel free to set to zero when done.
int IR_PIN = 2;
int RC5_INTERRUPT = 0;
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




void loop() {
   static int NextMainTakt;
   static int NextSecTakt;
   static int PrevMs;

   RcDispatch();

   // eens per miliseconde
   int ms = millis();
   if (ms != PrevMs) {  // miliseconde takt
      PrevMs = ms;  // bewaar huidige tijd
      SharpVoor   = SharpTakt(SHARP_VOOR_PIN,   SharpVoor_Gemiddelde,   400*148L);
      SharpRechts = SharpTakt(SHARP_RECHTS_PIN, SharpRechts_Gemiddelde, 400*148L);
      BlinkTakt();
   }

   // Main takt interval
   ms = millis();
   if ((ms - NextMainTakt) > 0) {
      NextMainTakt = ms + MAIN_TAKT_INTERVAL;  // zet tijd voor volgende interval

      // hier de periodieke acties voor deze interval
      Position.Takt(); // Lees & verwerk encoder data
      ProgrammaTakt(); // Voer (stapje van) geselecteerde programma uit
      Driver.Takt();    // stuur motoren aan

   }

   // Seconde interval
   ms = millis();
   if ((ms - NextSecTakt) > 0) {
      NextSecTakt = ms + 1000;  // zet tijd voor volgende interval

      // hier de periodieke acties voor deze interval
      
      if (SecondLoggingOn) {   
         // Optional logging, toggle with LIST button.
         //
         // printf("Encoder L/R TPrev %d/%d TAct: %d/%d, Count: %d/%d\n",
         //       EncoderLPeriode, EncoderRPeriode, EncoderL_LopendePeriode, EncoderR_LopendePeriode, EncoderLTeller, EncoderRTeller);
         EncoderPrint();
         Position.Print();
   
         //int Batterij = analogRead(BATTERIJ_PIN);
         //int Spanning = (int) (145L * Batterij / 960);  // 14.8 volt geeft waarde 964
         //printf("Batterij: %d (V * 10) (%d)\n", Spanning, Batterij);
   
         printf("Sharp %d %d %d %d\n", SharpVoor, SharpRechts, analogRead(SHARP_VOOR_PIN), analogRead(SHARP_RECHTS_PIN) );
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

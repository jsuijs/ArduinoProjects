//-----------------------------------------------------------------------------
// Planck.ino
//-----------------------------------------------------------------------------

// Dummy functie, zodat Arduino de vereiste prototypes plaatst.
void DummyFunctie() { }

// Robot-specifieke parameters
#include "MyRobot.h"


// include Position class & instantieer
TPosition Position;

// include Drive class & instantieer
TDrive Driver;
bool SecondLoggingOn = true;

int SharpLinks, SharpRechts;    // sharp meting - afstand in mm
long SharpLinks_Gemiddelde, SharpRechts_Gemiddelde;

int Lijn;  // 0..7, 3 bits. 0 = wit, 7 = zwart, 1 = links, 2 = midden, 4 = rechts

//#include <NewPing.h>
//#define TRIGGER_PIN  3
//#define ECHO_PIN     4
//#define MAX_DISTANCE 200
//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

unsigned long pingTimer;     // Holds the next ping time.

int UsDistance;


HardwareSerial Serial(USART1);

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


//#include <Servo.h>
//Servo myservo;  // create servo object to control a servo
// the setup routine runs once when you press reset:
void setup() {
   // start serial
   Serial.begin(115200);
//   Serial.println("Starten....\n");
   printf("Starten 2\n");
   MotorsSetup();

   EncoderSetup();

   // Link PinChange interrupt to RC5 reader.
   //attachInterrupt(RC5_INTERRUPT, Rc5Isr, CHANGE);

   pingTimer = millis(); // Start now.

   printf("Opstarten gereed.\n");
}

void loop() {
   static int NextMainTakt;
   static int NextSecTakt;
   static int PrevMs;

//   RcDispatch();

   // eens per miliseconde
   int ms = millis();
   if (ms != PrevMs) {  // miliseconde takt
      PrevMs = ms;  // bewaar huidige tijd
//      SharpLinks   = SharpTakt(SHARP_LINKS_PIN,   SharpLinks_Gemiddelde,   400*148L);
//      SharpRechts = SharpTakt(SHARP_RECHTS_PIN, SharpRechts_Gemiddelde, 400*148L);
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

   if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
      pingTimer += 50;           // Set the next ping time.
//      UsDistance = 10 * sonar.ping_cm(); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.

//      sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
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
         printf("encoder pins %d %d %d %d\n", digitalRead(ENCODER_L_PIN_A), digitalRead(ENCODER_L_PIN_B), digitalRead(ENCODER_R_PIN_A), digitalRead(ENCODER_R_PIN_B));
         //int Batterij = analogRead(BATTERIJ_PIN);
         //int Spanning = (int) (145L * Batterij / 960);  // 14.8 volt geeft waarde 964
         //printf("Batterij: %d (V * 10) (%d)\n", Spanning, Batterij);

//         printf("Sharp %d %d %d %d\n", SharpLinks, SharpRechts, (int)analogRead(SHARP_LINKS_PIN), (int)analogRead(SHARP_RECHTS_PIN) );
//         printf("Lijn: %d (%d %d %d)\n", Lijn, digitalRead(5), digitalRead(6), digitalRead(7));
//         printf("Sonar: %d\n", UsDistance);
      }
   }
}



//void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
//  // Don't do anything here!
//  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
//    // Here's where you can add code.
////    Serial.print("Ping: ");
////    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS resul
//    UsDistance = 10 * sonar.ping_result / US_ROUNDTRIP_CM; // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
////    UsDistance = 10 * sonar.ping_cm(); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
////    Serial.println("cm");
//  }
//  // Don't do anything here!
//}

void BlinkTakt()
{  static int Count;


   if (Count > 300) {
      digitalWrite(7, ! digitalRead(7));
      Count = 0;
   }
   Count ++;
}

//ISR(TIMER1_COMPA_vect)
//{
////  handle_interrupts(_timer1, &TCNT1, &OCR1A);
//  //static bool Toggle;
//  static bool State;
//
//  if (State) {
//    // set pin
//    digitalWrite(53, 1);
//    OCR1A = ServoTime * 2;  // time in us * 2
//    TCNT1 = 0;      // restart cycle
//    State = 0;
//  } else {
//    digitalWrite(53, 0);
//    OCR1A  = 40000;  // cycle time 20ms
//    State = 1;
//  }
//}
// Servo Stuff
//---------------------------------------------------------------------------------------

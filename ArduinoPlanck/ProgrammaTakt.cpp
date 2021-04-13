//-----------------------------------------------------------------------------
// ProgrammTakt.cpp
//-----------------------------------------------------------------------------
#include "MyRobot.h"

// prototypes (i.v.m. include uit ander project)
bool Rijden1Takt(bool Init);
bool UmbMark1(bool Init);
bool TTijdTakt(bool Init);
bool HeenEnWeerTakt(bool Init);
bool UmbMark1Takt(bool Init);
bool RandomRijdenTakt(bool Init);
// Global vars
int ProgrammaParam1;

int SharpLinks, SharpRechts;    // sharp meting - afstand in mm

//-----------------------------------------------------------------------------
// ProgrammaTakt - programma-keuze & aanroep van het programma
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ProgrammaTakt()
{
   static int Program, PrevProgram = -1;
   bool NewProgram = false;

   int ch = PfKeyGet();
   if (ch) {
      // knop ingedrukt

      CSerial.printf("Key: %d\n", ch);
      if (ch == -1) {
         Program = 0;           // reset, stop lopend programma / programma 'stilstaan'.
         NewProgram = true;     // ook re-trigger als state al 0 is
      } else {
         if (Program == 0) {    // andere pfkeys werken alleen als we stil staan
            Program = ch;
         }
      }
   }

   // rapporteer status bij state overgang
   if (PrevProgram != Program) {
      PrevProgram = Program;
      CSerial.printf("Programma: %d\n", Program);
      NewProgram = true;
   }

   // Roep actieve programma 1 t/m 12  aan.
   switch(Program) {

      case 0 : { // Programma: stil staan
         if (NewProgram) {
            Driver.Pwm(0, 0); // only on entry, so CLI-commands can be used in this state.
         }
      }
      break;

      case 1 : { // Programma: rijden1
         if (Rijden1Takt(NewProgram)) {
            Program = 0;
         }
      }
      break;

      case 2 : { // Programma: max_pwm
         Driver.Pwm(155, 155);
      }
      break;

      case 3 : { // Programma: speed
         Driver.SpeedLR(300, 300);
      }
      break;

      case 4 : { // Programma: speed
         Driver.SpeedLR(50, -50);
      }
      break;

      case 5 : { // Programma: UmbMark CCW
         ProgrammaParam1 = 1;             // set CCW
         if (UmbMark1Takt(NewProgram)) {    // call StateMachine
            Program = 0;                    // Statemachine returns true => program done.
         }
      }
      break;

      case 6 : { // Programma: UmbMark CW
         ProgrammaParam1 = -1;            // set CW
         if (UmbMark1Takt(NewProgram)) {    // call StateMachine
            Program = 0;                    // Statemachine returns true => program done.
         }
      }
      break;

      case 7 : { // Programma:
      }
      break;

      case 8 : { // Programma: UmbMark CW
         if (DetectBlikTakt(NewProgram)) {  // call StateMachine
            Program = 0;                    // Statemachine returns true => program done.
         }
      }
      break;

      case 9 : { // Programma: Heen en Weer
         if (HeenEnWeerTakt(NewProgram)) {  // call StateMachine
            Program = 0;                    // Statemachine returns true => program done.
         }
      }
      break;

      case 10 : { // Programma: ttijd
         if (TTijdTakt(NewProgram)) {       // call StateMachine
            Program = 0;                    // Statemachine returns true => program done.
         }
      }
      break;

      case 11 : { // Programma:
         if (RandomRijdenTakt(NewProgram)) Program = 0;
      }
      break;

      case 12 : { // Programma: test1
         Driver.Rotate(90);
//         State = 0;
      }
      break;

      default : {
         CSerial.printf("ProgrammaTakt: ongeldig programma %d\n", Program);
         Program = 0;
      }
      break;
   } // einde van switch
}



//-----------------------------------------------------------------------------
// Rijden1Takt - pwm trapje, print pwm + toerental
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool Rijden1Takt(bool Init)
{
   static int Step;
   static bool Oplopend;
   static int Slow;

   if (Init) {
      Step = 0;
      Oplopend = true;
      Slow = 0;
   }

   if (Slow == 0) {
      if (Oplopend) {
         Step +=2;
      } else {
         Step -=2;
      }
      Slow = 10;
   } else {
      Slow --;
   }

    if (Step >  255) Oplopend = false;
    if (Step < -255) Oplopend = true;

    Driver.Pwm(Step, Step);
    CSerial.printf("Pwm: %d, Speed: %d / %d\n", Step, Position.ActSpeedL, Position.ActSpeedR);
    Position.Print();

    if (Oplopend && (Step == 0)) {
      return true; // Klaar
   }

   return false;  // Nog niet klaar.
}



#define UMB_MARK_AFSTAND 800    // mm
#define UMB_MARK_SPEED 300    // mm/sec

//-----------------------------------------------------------------------------
// MSM_UmbMark1 -
//-----------------------------------------------------------------------------
// Bij UmbMark rijdt de robot een vierkant met de klok mee en een vierkant
// tegen de klok in. Bij aankomst wordt de verschuiving in X-richting opgemeten
// en op basis van deze twee getallen (delta-X clockwise & counterclockwise)
// kan worden bepaald wat de afwijking van de wielbasis-calibratie en de
// afwijking door wielgrootte is.
//
// var ProgrammaParam1 = 1 (CCW) of -1 (CW)
//
//-----------------------------------------------------------------------------
bool UmbMark1Takt(bool Init)
{  static int State, PrevState;
   bool NewState = false;

   if (Init) {
      State  = 0;
      PrevState = -1;
   }

   if (PrevState != State) {
      PrevState = State;
      NewState = true;
      CSerial.printf("UmbMark1 state %d\n", State);
   }

   switch (State) {
      case 0 :    // Rij naar X, 0
            if (NewState) {
               Driver.XY(UMB_MARK_AFSTAND, 0, UMB_MARK_SPEED, 0);  // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }

            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 1 :    // Draai naar +90 of -90, afhankelijk van MissionHandler.ParamGet(0)
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90 * ProgrammaParam1); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 2 :    // Rij naar X, Y of X, -Y, afhankelijk van MissionHandler.ParamGet(0)
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY( UMB_MARK_AFSTAND,
                        UMB_MARK_AFSTAND * ProgrammaParam1,
                        UMB_MARK_SPEED,
                        0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 3 :    // Draai naar 180 graden
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 4 :    // Rij naar 0, Y of 0, -Y, afhankelijk van MissionHandler.ParamGet(0)
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY( 0,
                        UMB_MARK_AFSTAND * ProgrammaParam1,
                        UMB_MARK_SPEED,
                        0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 5 :    // Draai naar -90 of +90, afhankelijk van MissionHandler.ParamGet(0)
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(-90 * ProgrammaParam1); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 6 :    // Rij naar 0, 0
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY(0, 0, UMB_MARK_SPEED, 0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 7 :    // Draai naar 0 graden
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(0); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               return true;   // Geef aan dat de missie klaar is.
                              // De Msm zal nu niet meer aangeroepen worden.
            }
         break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}

#define HW_SNELHEID 500
// HeenEnWeer state machine
bool HeenEnWeerTakt(bool Init)
{  static int State, PrevState;
   bool NewState = false;
   int x;

   if (Init) {
      State  = 0;
      PrevState = -1;
   }

   if (PrevState != State) {
      PrevState = State;
      NewState = true;
      CSerial.printf("HW state %d\n", State);
   }

   CSerial.printf("hw %d %d %d %d\n", State, Driver.SollSpeedL, Driver.SollSpeedR, SharpRechts);

   switch (State) {
      case 0 :    // Volg wand naar vak B
            if (NewState) {
               Position.Reset();
            }
            x = (250 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(HW_SNELHEID, x);  // Speed, Heading

//            if (UsDistance < 500) { // Als we de wand voor ons zien
//               State++; // naar volgende state
//            }
         break;

      case 1 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 2 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 3 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 4 :    // Terug naar startpunt
            x = 180 + (SharpLinks - 250)/10;  // wand volgen
            Driver.SpeedHeading(HW_SNELHEID, x);  // Speed, Heading

//            if (UsDistance < 500) { // Als we de wand voor ons zien
//               State++; // naar volgende state
//            }
         break;

      case 5 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}


#define TT_SNELHEID 300
// TTijd state machine
bool TTijdTakt(bool Init)
{  static int State, PrevState;
   bool NewState = false;
   int x;

   if (Init) {
      State  = 0;
      PrevState = -1;
   }

   if (PrevState != State) {
      PrevState = State;
      NewState = true;
      CSerial.printf("TTijd state %d\n", State);
   }

   switch (State) {
      case 0 :    // Volg wand naar vak B
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(TT_SNELHEID, x);  // Speed, Heading

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               State++; // naar volgende state
            }
         break;

      case 1 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 2 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 3 :    // Volg wand in vak B
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(TT_SNELHEID, x + 90);  // Speed, Heading
            CSerial.printf("SharpRechts: %d\n", SharpRechts);

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               State++; // naar volgende state
            }
         break;

      case 4 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 5 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 6 :    // Volg wand uit vak B
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(TT_SNELHEID, x + 180);  // Speed, Heading

            if (SharpRechts > 400) { // Als we rechts geen wand meer zien
               State++; // naar volgende state
            }
         break;


      case 7 :    // Nog klein stukje rechtdoor
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY(Position.XPos-280, Position.YPos, TT_SNELHEID, 0);
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 8 :    // draai richting C
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 9 :    // Volg wand naar vak C
            x = (200 - SharpRechts)/10;  // wand volgen
            x = Clip(x, -5, 5);
            Driver.SpeedHeading(TT_SNELHEID, x + 90);  // Speed, Heading
            CSerial.printf("Sharp: %d, x: %d\n", SharpRechts, x);

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               State++; // naar volgende state
            }
         break;

      case 10 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 11 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 12 :    // Volg wand in vak C
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(TT_SNELHEID, x + 180);  // Speed, Heading

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               State++; // naar volgende state
            }
         break;

      case 13 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 14 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(270); // Heading (in graden)
            }

            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 15 :    // Volg wand uit vak C
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(TT_SNELHEID, x + 270);  // Speed, Heading

            if (SharpRechts > 400) { // Als we rechts geen wand meer zien
               State++; // naar volgende state
            }
         break;

      case 16 :    // Nog een klein stukje rechtdoor
            if (NewState) {
               // voor het eerst in deze state
               Driver.XY(Position.XPos, Position.YPos-280, TT_SNELHEID, 0); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 17 :    // Draai
            if (NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }

            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      case 18 :    // Volg wand naar vak A
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(TT_SNELHEID, x + 180);  // Speed, Heading

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               return true; // done
            }
         break;

      case 19 :    // Stop
            if (NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               State++; // naar volgende state
            }
         break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}

//-----------------------------------------------------------------------------
// DetectBlikTakt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool DetectBlikTakt(bool Init)
{  static int State, PrevState;
   bool NewState = false;
   static int Afstanden[90];
   static int Target;

   static int StartRobotHoek;

   if (Init) {
      State  = 0;
      PrevState = -1;
   }

   if (PrevState != State) {
      PrevState = State;
      NewState = true;
      CSerial.printf("Template state %d\n", State);
   }

   switch (State) {
      case 0 : {    // draai naar rechts
         if (NewState) {
            StartRobotHoek = Position.Hoek;
            Driver.SpeedLR(100, -100);
         }
//         CSerial.printf("aaHoek: %d, Sonar: %d\n", Position.Hoek, UsDistance);
//         if (Driver.IsDone()) { // Als de beweging klaar is
//            State++; // naar volgende state
//         }
         if (Position.Hoek - StartRobotHoek < -45 ) State ++;
      }
      break;

      case 1 :  {  // scan naar links
         if (NewState) {
            Driver.SpeedLR(-50, 50);
            for (int x=0; x< 90; x++) Afstanden[x] = 999;
         }
         int Ix = Position.Hoek + 45 - StartRobotHoek;
         if ((Ix >= 0) && (Ix < 90)) {
//             Afstanden[Ix] = UsDistance;
         }
//         CSerial.printf("aaHoek: %d, Sonar: %d\n", Position.Hoek, UsDistance);
         if (Position.Hoek - StartRobotHoek > 45 ) State ++;
      }
      break;

      case 2 : {   // draai naar blik
         if (NewState) {
            int Min = Afstanden[0];
            int First = 999, Last = 999;

            for (int x=0; x< 90; x++) {
               if (Afstanden[x] < Min) Min = Afstanden[x];
            }

            for (int x=0; x< 90; x++) {
               if (Afstanden[x] < (Min + 150)) {
                  // blik in zicht
                  if (First == 999) First = x;
                  Last = x;
               }
            }
            CSerial.printf("Min: %d, First: %d, Last: %d", Min, First, Last);
            Target = (First + Last) / 2 - 45;
            Driver.SpeedLR(100, -100);
         }

         if (Position.Hoek - StartRobotHoek < Target ) State ++;
      }
      break;

      case 3 : {    // rij naar blik
         if (NewState) {
            Driver.SpeedHeading(200, Target + StartRobotHoek);
//            GrijperMagneetVast(true);
         }
//         CSerial.printf("aaHoek: %d, Sonar: %d\n", Position.Hoek, UsDistance);
//         if (Driver.IsDone()) { // Als de beweging klaar is
//            State++; // naar volgende state
//         }
//         if (UsDistance < 40) State ++;
      }
         break;



      case 4 : {    // rij terug
         static int EndValue;

         int OdoL, OdoR, OdoT;
         Position.OdoGet(OdoL, OdoR, OdoT) ;

         if (NewState) {
            Driver.SpeedHeading(-200, Target + StartRobotHoek);
             EndValue = OdoT + 500;

         }
         CSerial.printf("OdoT: %d\n", OdoT);
         if ((OdoT - EndValue) > 0) {
//              GrijperMagneetVast(false);
           State ++;
         }
      }
      break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}


//-----------------------------------------------------------------------------
// TemplateTakt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//bool TemplateTakt(bool Init)
//{
//    static int State, PrevState;
//    bool NewState = false;
//
//    if (Init) {
//       State  = 0;
//       PrevState = -1;
//    }
//
//    if (PrevState != State) {
//       PrevState = State;
//       NewState = true;
//       CSerial.printf("Template state %d\n", State);
//    }
//
//    switch (State) {
//       case 0 : {  //
//          if (NewState) {
//             Driver.XY(UMB_MARK_AFSTAND, 0, UMB_MARK_SPEED, 0);  // X, Y, Speed, EndSpeed - alles in mm(/sec)
//          }
//
//          if (Driver.IsDone()) { // Als de beweging klaar is
//             State++; // naar volgende state
//          }
//       }
//       break;
//
//       default : return true;  // error => mission end
//    }
//    return false;  // mission nog niet gereed
//}




int count_l, count_r, AfstBediening;
int Lidar_LBB;          //Lpp.Array[0]av LD4 70<>90 gr
int Lidar_LB;          //Lpp.Array[1]
int Lidar_LLLV;        //Lpp.Array[2]
int Lidar_LLV;         //Lpp.Array[3]
int Lidar_LV;          //Lpp.Array[4]
int Lidar_V;           //Lpp.Array[5]
int Lidar_RV;          //Lpp.Array[6]
int Lidar_RRV;         //Lpp.Array[7]
int Lidar_RRRV;        //Lpp.Array[8]
int Lidar_RB;          //Lpp.Array[9]
int Lidar_RBB;          //Lpp.Array[10]av LD4 270<>290 gr
// ** Extra sensoren AV
int Lidar_A;            // Lpp.Sensor(0) Achter
int Lidar_grV;          // Scannen(1) Hoek van 90 graden + 180 gr tot 270 graden
int Lidar_90V;          // Scannen(2) Korste Afstand van 135 graden + 90 gr tot 225 graden
// ** LockDown 8-slalom
int Lidar_Blik_L;       // Scannen(3)Rond Blik Linksom
int Lidar_Blik_LV;      // Scannen(4)Rond Blik Linksom
int Lidar_Blik_V;       // Scannen(5)Rond Blik Rechtsom
int Lidar_Blik_RV;      // Scannen(6)Rond Blik Rechtsom
int Lidar_Blik_R;       // Scannen(7)Rond Blik Rechtsom
int Setpunt_r;           // tussenstand encoder rechts
int Setpunt_l;          // tussenstand encoder links
int Lid_Max;            // Meetwaarde beperken

//-----------------------------------------------------------------------------
// RandomRijdenTakt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool RandomRijdenTakt(bool Init)
{
    static int State, PrevState;
    bool NewState = false;

    if (Init) {
       State  = 0;
       PrevState = -1;
    }

    if (PrevState != State) {
       CSerial.printf("RandomRijden state %d -> %d\n", PrevState, State);
       PrevState = State;
       NewState  = true;
    }

    switch (State) {

      //----------------------------------------------------------------------
      //      *** MaxonRobot - Slalom=8 - Rijden LockDown Challenge !!! 02-09-2020
      //----------------------------------------------------------------------
      case 0 : { // LIDAR-STARTEN 8 - Rijden LockDown Challenge
         if (NewState) {
            CSerial.printf(" case 500 = START LIDAR.\n");
            Motors(0, 0);
            Lpp.Start();
         }
         // Condities
         if ((AfstBediening == 12) || (AfstBediening == 5)) { //av 12 = 0/1 knop of 5 Stoppen
            AfstBediening = 0;
            State = 990;
            Motors(0, 0);
            CSerial.println("MaxonRobot -5- STOPPEN State = 0 .\n");
         }
         // Start Lidar
         if (AfstBediening == 13) {          // Knop -Mute-
            State = 501;              // 8-Rijden LockDown Challenge
         }
         if (AfstBediening == 10) {  //10 = Knop <<->> - Random rijden
            State = 601;              //Random rijden in bak
         }
      } // EINDE CASE-500 Slalom=8 - LIDAR STARTEN Rijden LockDown Challenge
      break;

      //----------------------------------------------------------------------
      //      *** Maxon2 Robot - Random rijden
      //----------------------------------------------------------------------
      case 601 : { // Random rijden

         if (NewState) {
            CSerial.println(" RANDOM RIJDEN.\n");
            CSerial.println(" case 601.\n");
         }

         // Condities
         if ((AfstBediening == 12) || (AfstBediening == 5)) { //av Stoppen
            AfstBediening = 0;
            State = 990;
            Motors(0, 0);
            CSerial.println("MaxonRobot -5- STOPPEN State = 0 .\n");
         }

         Lid_Max = 500;          // Lidar Meetwaarde beperken
         //if (Lidar_grV > Lid_Max)   Lidar_grV = Lid_Max;         // 90><180 gr valse meting
         //if (Lidar_90V > Lid_Max)   Lidar_90V = Lid_Max;         // 135><225 gr valse meting
         if (Lidar_Blik_L > Lid_Max)  Lidar_Blik_L = Lid_Max;    // 70><110 gr valse meting
         if (Lidar_Blik_LV > Lid_Max)  Lidar_Blik_LV = Lid_Max;  // 110><150 gr valse meting
         if (Lidar_Blik_V > Lid_Max)  Lidar_Blik_V = Lid_Max;    // 150><210 gr valse meting
         if (Lidar_Blik_RV > Lid_Max)  Lidar_Blik_RV = Lid_Max;  // 210><250 gr valse meting
         if (Lidar_Blik_R > Lid_Max)  Lidar_Blik_R = Lid_Max;    // 250><290 gr valse meting

         if (  Lidar_Blik_V < 200) {
            Setpunt_r = count_r;
            Setpunt_l = count_l;
            // print a random number from 0 to 6
            int randNumber = random(7);
            CSerial.println(randNumber);
            if ((randNumber == 1) || (randNumber == 3) || (randNumber == 5)) {
               State = 603;      // 180 gr R/Om draaien
            }
            else {
               State = 604;      // 180 gr L/Om draaien
            }
         }
         if ((  Lidar_Blik_LV < 200) && (Lidar_Blik_V < 250) && (Lidar_Blik_RV < 200)) {
            Setpunt_r = count_r;
            Setpunt_l = count_l;
            State = 602;      // 180 gr R/Om draaien
         }
         if ((Lidar_Blik_LV < 200) || (Lidar_Blik_RV > Lidar_Blik_LV)) {
            Motors(70, 40);   // Rechts afdraaien
            break;
         }
         if ((Lidar_Blik_RV < 200) || (Lidar_Blik_LV > Lidar_Blik_RV)) {
            Motors(40, 70);   //Links afdraaien
            break;
         }
         Motors(60, 63);   // default rechtuit
      }            // EINDE CASE-601 Random rijden
      break;

      //----------------------------------------------------------------------
      case 602 : { // 180 gr RECHTSOM draaien - Random rijden

         if (NewState) {
            CSerial.println(" Lidar Discover.\n");
            CSerial.println(" case 602.\n");
         }
         // Condities
         if ((AfstBediening == 12) || (AfstBediening == 5)) { //av Stoppen
            AfstBediening = 0;
            State = 990;
            Motors(0, 0);
            CSerial.println("MaxonRobot -5- STOPPEN State = 0 .\n");
         }
         Motors(50, -50);
         if ((count_l >= Setpunt_l + 2276) && (count_r <= Setpunt_r - 2276)) { //1138.97x2 = 180 graden rechtsom draaien
            Motors(0, 0);
            count_l = 0 , count_r = 0;
            State = 601;
         }
      }            // EINDE CASE-602 Slalom=8 - Rijden LockDown Challenge
      break;

      //----------------------------------------------------------------------
      case 603 : { // 90 gr RECHTSOM draaien - Random rijden

         if (NewState) {
            CSerial.println(" Lidar Discover.\n");
            CSerial.println(" case 603.\n");
         }
         // Condities
         if ((AfstBediening == 12) || (AfstBediening == 5)) { //av Stoppen
            AfstBediening = 0;
            State = 990;
            Motors(0, 0);
            CSerial.println("MaxonRobot -5- STOPPEN State = 0 .\n");
         }
         Motors(50, -50);
         if ((count_l >= Setpunt_l + 1138) && (count_r <= Setpunt_r - 1138)) { //1138.97 = 90 graden rechtsom draaien
            Motors(0, 0);
            count_l = 0 , count_r = 0;
            State = 601;
         }
      }            // EINDE CASE-603 Random rijden
      break;

     //----------------------------------------------------------------------
      case 604 : { // 90 gr LINKSOM draaien - Random rijden

         if (NewState) {
            CSerial.println(" Lidar Discover.\n");
            CSerial.println(" case 604.\n");
         }

         // Condities
         if ((AfstBediening == 12) || (AfstBediening == 5)) { //av Stoppen
            AfstBediening = 0;
            State = 990;
            Motors(0, 0);
            CSerial.println("MaxonRobot -5- STOPPEN State = 0 .\n");
         }
         Motors(-50, 50);
         if ((count_l >= Setpunt_l - 1138) && (count_r <= Setpunt_r + 1138)) { //1138.97 = 90 graden linksom draaien
            Motors(0, 0);
            count_l = 0 , count_r = 0;
            State = 601;
         }
      }            // EINDE CASE-604 Random rijden
      break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}
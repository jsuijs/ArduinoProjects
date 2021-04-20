//-----------------------------------------------------------------------------
// ProgrammTakt.cpp
//-----------------------------------------------------------------------------
#include "RobotSettings.h"
#include "Libs/MyRobot.h"

// prototypes
bool Rijden1Takt(bool Init);
bool MissieUmbMark1(TState &S);
bool MissieTTijd(TState &S);
bool MissieHeenEnWeer(TState &S);
bool MissieRandomRijden(TState &S);
bool MissieDetectBlik(TState &S);

int SharpLinks, SharpRechts;    // sharp meting - afstand in mm

TState GlobS;  // Statemachine voor missie

//-----------------------------------------------------------------------------
// ProgrammaTakt - programma-keuze & aanroep van het programma
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ProgrammaTakt()
{  static TState Program;

   int ch = PfKeyGet();
   if (ch) {
      // knop ingedrukt

      CSerial.printf("Key: %d\n", ch);
      if (ch == -1) {
         Program.Reset();     // reset, stop lopend programma / programma 'stilstaan'.
      } else {
         if (Program.State == 0) {    // andere pfkeys werken alleen als we stil staan
            Program.State = ch;
         }
      }
   }

   Program.Update("Programma", Flags.IsSet(1));
   if (Program.NewState) GlobS.Reset();   // reset statemachine van missie zelf.

   // Roep actieve programma 1 t/m 12  aan.
   switch(Program.State) {

      case 0 : { // Programma: stil staan
         if (Program.NewState) {
            Driver.Pwm(0, 0); // only on entry, so CLI-commands can be used in this state.
            Lpp.Stop();
         }
      }
      break;

      case 1 : { // Programma: rijden1
         if (Rijden1Takt(Program.NewState)) Program.State = 0;
      }
      break;

      case 2 : {  // Programma:
      }
      break;

      case 3 : {  // Programma:
      }
      break;

      case 4 : { // Programma:
      }
      break;

      case 5 : { // Programma: UmbMark CCW
         GlobS.Param1 = 1;                // set CCW
         if (MissieUmbMark1(GlobS)) Program.State = 0;
      }
      break;

      case 6 : { // Programma: UmbMark CW
         GlobS.Param1 = -1;               // set CW
         if (MissieUmbMark1(GlobS)) Program.State = 0;
      }
      break;

      case 7 : { // Programma:
      }
      break;

      case 8 : { // Programma: UmbMark CW
         if (MissieDetectBlik(GlobS)) Program.State = 0;
      }
      break;

      case 9 : { // Programma: Heen en Weer
         GlobS.Param1 = 500;  // speed
         if (MissieHeenEnWeer(GlobS)) Program.State = 0;
      }
      break;

      case 10 : { // Programma: ttijd
         GlobS.Param1 = 300;  // speed
         if (MissieTTijd(GlobS)) Program.State = 0;
      }
      break;

      case 11 : { // Programma:
         if (MissieRandomRijden(GlobS)) Program.State = 0;
      }
      break;

      case 12 : { // Programma: test1
         Driver.Rotate(90);
      }
      break;

      default : {
         CSerial.printf("ProgrammaTakt: ongeldig programma %d\n", Program);
         Program.Reset();
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
// MissieUmbMark1 -
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
bool MissieUmbMark1(TState &S)
{
   S.Update("UmbMark1", Flags.IsSet(2));

   switch (S.State) {
      case 0 :    // Rij naar X, 0
            if (S.NewState) {
               Driver.XY(UMB_MARK_AFSTAND, 0, UMB_MARK_SPEED, 0);  // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }

            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 1 :    // Draai naar +90 of -90, afhankelijk van MissionHandler.ParamGet(0)
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90 * S.Param1); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 2 :    // Rij naar X, Y of X, -Y, afhankelijk van MissionHandler.ParamGet(0)
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.XY( UMB_MARK_AFSTAND,
                        UMB_MARK_AFSTAND * S.Param1,
                        UMB_MARK_SPEED,
                        0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 3 :    // Draai naar 180 graden
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 4 :    // Rij naar 0, Y of 0, -Y, afhankelijk van MissionHandler.ParamGet(0)
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.XY( 0,
                        UMB_MARK_AFSTAND * S.Param1,
                        UMB_MARK_SPEED,
                        0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 5 :    // Draai naar -90 of +90, afhankelijk van MissionHandler.ParamGet(0)
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(-90 * S.Param1); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 6 :    // Rij naar 0, 0
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.XY(0, 0, UMB_MARK_SPEED, 0); // X, Y, Speed, EndSpeed - alles in mm(/sec)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 7 :    // Draai naar 0 graden
            if (S.NewState) {
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

//-----------------------------------------------------------------------------
// MissieHeenEnWeer - state machine
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool MissieHeenEnWeer(TState &S)
{  int x;

   S.Update("HW", Flags.IsSet(2));

   CSerial.printf("hw %d %d %d %d\n", S.State, Driver.SollSpeedL, Driver.SollSpeedR, SharpRechts);

   switch (S.State) {

      case 0 : {  // Volg wand naar vak B
         if (S.NewState) {
            Position.Reset();
         }
         x = (250 - SharpRechts)/10;  // wand volgen
         Driver.SpeedHeading(S.Param1, x);  // Speed, Heading

//            if (UsDistance < 500) { // Als we de wand voor ons zien
//               State++; // naar volgende state
//            }
      }
      break;

      case 1 : {  // Stop
         if (S.NewState) {
            // voor het eerst in deze state
            Driver.Stop();
         }
         if (Driver.IsDone()) { // Als de beweging klaar is
            S.State++; // naar volgende state
         }
      }
      break;

      case 2 : {  // Draai
         if (S.NewState) {
            // voor het eerst in deze state
            Driver.Rotate(90); // Heading (in graden)
         }
         if (Driver.IsDone()) { // Als de beweging klaar is
            S.State++; // naar volgende state
         }
      }
      break;

      case 3 : {  // Draai
         if (S.NewState) {
            // voor het eerst in deze state
            Driver.Rotate(180); // Heading (in graden)
         }
         if (Driver.IsDone()) { // Als de beweging klaar is
            S.State++; // naar volgende state
         }
      }
      break;

      case 4 : {  // Terug naar startpunt
         x = 180 + (SharpLinks - 250)/10;  // wand volgen
         Driver.SpeedHeading(S.Param1, x);  // Speed, Heading

//            if (UsDistance < 500) { // Als we de wand voor ons zien
//               State++; // naar volgende state
//            }
      }
      break;

      case 5 : {  // Stop
         if (S.NewState) {
            // voor het eerst in deze state
            Driver.Stop();
         }
         if (Driver.IsDone()) { // Als de beweging klaar is
            S.State++; // naar volgende state
         }
      }
      break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}

//-----------------------------------------------------------------------------
// MissieTTijd - state machine
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool MissieTTijd(TState &S)
{  int x;

   S.Update("TTijd", Flags.IsSet(2));

   switch (S.State) {
      case 0 :    // Volg wand naar vak B
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(S.Param1, x);  // Speed, Heading

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               S.State++; // naar volgende state
            }
         break;

      case 1 :    // Stop
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 2 :    // Draai
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 3 :    // Volg wand in vak B
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(S.Param1, x + 90);  // Speed, Heading
            CSerial.printf("SharpRechts: %d\n", SharpRechts);

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               S.State++; // naar volgende state
            }
         break;

      case 4 :    // Stop
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 5 :    // Draai
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 6 :    // Volg wand uit vak B
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(S.Param1, x + 180);  // Speed, Heading

            if (SharpRechts > 400) { // Als we rechts geen wand meer zien
               S.State++; // naar volgende state
            }
         break;


      case 7 :    // Nog klein stukje rechtdoor
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.XY(Position.XPos-280, Position.YPos, S.Param1, 0);
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 8 :    // draai richting C
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(90); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 9 :    // Volg wand naar vak C
            x = (200 - SharpRechts)/10;  // wand volgen
            x = Clip(x, -5, 5);
            Driver.SpeedHeading(S.Param1, x + 90);  // Speed, Heading
            CSerial.printf("Sharp: %d, x: %d\n", SharpRechts, x);

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               S.State++; // naar volgende state
            }
         break;

      case 10 :    // Stop
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 11 :    // Draai
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 12 :    // Volg wand in vak C
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(S.Param1, x + 180);  // Speed, Heading

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               S.State++; // naar volgende state
            }
         break;

      case 13 :    // Stop
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 14 :    // Draai
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(270); // Heading (in graden)
            }

            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 15 :    // Volg wand uit vak C
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(S.Param1, x + 270);  // Speed, Heading

            if (SharpRechts > 400) { // Als we rechts geen wand meer zien
               S.State++; // naar volgende state
            }
         break;

      case 16 :    // Nog een klein stukje rechtdoor
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.XY(Position.XPos, Position.YPos-280, S.Param1, 0); // Heading (in graden)
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 17 :    // Draai
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Rotate(180); // Heading (in graden)
            }

            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      case 18 :    // Volg wand naar vak A
            x = (200 - SharpRechts)/10;  // wand volgen
            Driver.SpeedHeading(S.Param1, x + 180);  // Speed, Heading

            if (SharpLinks < 300) { // Als we de wand voor ons zien
               return true; // done
            }
         break;

      case 19 :    // Stop
            if (S.NewState) {
               // voor het eerst in deze state
               Driver.Stop();
            }
            if (Driver.IsDone()) { // Als de beweging klaar is
               S.State++; // naar volgende state
            }
         break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}

//-----------------------------------------------------------------------------
// MissieDetectBlik -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool MissieDetectBlik(TState &S)
{
   static int Afstanden[90];
   static int Target;

   static int StartRobotHoek;


   S.Update("DetectBlik", Flags.IsSet(2));

   switch (S.State) {
      case 0 : {    // draai naar rechts
         if (S.NewState) {
            StartRobotHoek = Position.Hoek;
            Driver.SpeedLR(100, -100);
         }
//         CSerial.printf("aaHoek: %d, Sonar: %d\n", Position.Hoek, UsDistance);
//         if (Driver.IsDone()) { // Als de beweging klaar is
//            State++; // naar volgende state
//         }
         if (Position.Hoek - StartRobotHoek < -45 ) S.State ++;
      }
      break;

      case 1 :  {  // scan naar links
         if (S.NewState) {
            Driver.SpeedLR(-50, 50);
            for (int x=0; x< 90; x++) Afstanden[x] = 999;
         }
         int Ix = Position.Hoek + 45 - StartRobotHoek;
         if ((Ix >= 0) && (Ix < 90)) {
//             Afstanden[Ix] = UsDistance;
         }
//         CSerial.printf("aaHoek: %d, Sonar: %d\n", Position.Hoek, UsDistance);
         if (Position.Hoek - StartRobotHoek > 45 ) S.State ++;
      }
      break;

      case 2 : {   // draai naar blik
         if (S.NewState) {
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

         if (Position.Hoek - StartRobotHoek < Target ) S.State ++;
      }
      break;

      case 3 : {    // rij naar blik
         if (S.NewState) {
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

         if (S.NewState) {
            Driver.SpeedHeading(-200, Target + StartRobotHoek);
             EndValue = OdoT + 500;

         }
         CSerial.printf("OdoT: %d\n", OdoT);
         if ((OdoT - EndValue) > 0) {
//              GrijperMagneetVast(false);
           S.State ++;
         }
      }
      break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}

int AfstBediening;

//-----------------------------------------------------------------------------
// RandomRijdenTakt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool MissieRandomRijden(TState &S)
{
   S.Update("RandomRijden", Flags.IsSet(2));

//   int Lidar_A = Lpp.Sensor[0].Distance;         // Afstand achterzijde
//   int Lidar_grV = Lpp.Sensor[1].Degrees32 / 32; // Scannen(1) Hoek van 90 graden + 180 gr tot 270 graden
//   int Lidar_90V = Lpp.Sensor[2].Distance;       // Scannen(2) Korste Afstand van 135 graden + 90 gr tot 225 graden
   int Lidar_Blik_L  = Lpp.Sensor[3].Distance;  // Scannen(3) Korste Afstand van 70 graden + 40 gr tot 110 graden
   int Lidar_Blik_LV = Lpp.Sensor[4].Distance;  // Scannen(4) Korste Afstand van 110 graden + 40 gr tot 150 graden
   int Lidar_Blik_V  = Lpp.Sensor[5].Distance;  // Scannen(5) Korste Afstand van 150 graden + 60 gr tot 210 graden
   int Lidar_Blik_RV = Lpp.Sensor[6].Distance;  // Scannen(6) Korste Afstand van 210 graden + 40 gr tot 250 graden
   int Lidar_Blik_R  = Lpp.Sensor[7].Distance;  // Scannen(7) Korste Afstand van 250 graden + 40 gr tot 290 graden

   switch (S.State) {

      //----------------------------------------------------------------------
      //      *** MaxonRobot - Slalom=8 - Rijden LockDown Challenge !!! 02-09-2020
      //----------------------------------------------------------------------
      case 0 : { // LIDAR-STARTEN 8 - Rijden LockDown Challenge
         if (S.NewState) {
            Driver.Pwm(0, 0);
            Lpp.Start();
         }

         if (S.StateTime() > 2000) {   //Wacht op start lidar
            S.State = 601;              //Random rijden in bak
         }
      }
      break;

      //----------------------------------------------------------------------
      //      *** Maxon2 Robot - Random rijden
      //----------------------------------------------------------------------
      case 601 : { // Random rijden

         if (S.NewState) {
         }

         int Lid_Max = 500;          // Lidar Meetwaarde beperken
         //if (Lidar_grV > Lid_Max)   Lidar_grV = Lid_Max;        // 90><180 gr valse meting
         //if (Lidar_90V > Lid_Max)   Lidar_90V = Lid_Max;        // 135><225 gr valse meting
         if (Lidar_Blik_L  > Lid_Max)  Lidar_Blik_L  = Lid_Max;   // 70><110 gr valse meting
         if (Lidar_Blik_LV > Lid_Max)  Lidar_Blik_LV = Lid_Max;   // 110><150 gr valse meting
         if (Lidar_Blik_V  > Lid_Max)  Lidar_Blik_V  = Lid_Max;   // 150><210 gr valse meting
         if (Lidar_Blik_RV > Lid_Max)  Lidar_Blik_RV = Lid_Max;   // 210><250 gr valse meting
         if (Lidar_Blik_R  > Lid_Max)  Lidar_Blik_R  = Lid_Max;   // 250><290 gr valse meting

         if (Lidar_Blik_V < 200) {
            // print a random number from 0 to 6
            int randNumber = random(2);
            CSerial.println(randNumber);
            if (randNumber == 1) {
               S.State = 603;      // 180 gr R/Om draaien
            }
            else {
               S.State = 604;      // 180 gr L/Om draaien
            }
         }
         if ((  Lidar_Blik_LV < 200) && (Lidar_Blik_V < 250) && (Lidar_Blik_RV < 200)) {
            S.State = 602;      // 180 gr R/Om draaien
         }
         if ((Lidar_Blik_LV < 200) || (Lidar_Blik_RV > Lidar_Blik_LV)) {
            Driver.SpeedLR(110, 60);   // Rechts afdraaien
            break;
         }
         if ((Lidar_Blik_RV < 200) || (Lidar_Blik_LV > Lidar_Blik_RV)) {
            Driver.SpeedLR(60, 110);   //Links afdraaien
            break;
         }
         Driver.SpeedLR(110, 110);   // default rechtuit
      }
      break;

      //----------------------------------------------------------------------
      case 602 : { // 180 gr RECHTSOM draaien - Random rijden

         if (S.NewState) {
            Driver.RotateRel(-180);
         }
         if (Driver.IsDone()) S.State = 601;
      }
      break;

      //----------------------------------------------------------------------
      case 603 : { // 90 gr RECHTSOM draaien - Random rijden

         if (S.NewState) {
            Driver.RotateRel(-90);
         }
         if (Driver.IsDone()) S.State = 601;
      }
      break;

     //----------------------------------------------------------------------
      case 604 : { // 90 gr LINKSOM draaien - Random rijden

         if (S.NewState) {
            Driver.RotateRel(90);
         }
         if (Driver.IsDone()) S.State = 601;
      }
      break;

      default : return true;  // error => mission end
   }
   return false;  // mission nog niet gereed
}

////-----------------------------------------------------------------------------
//// MissieTemplate -
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
//bool MissieTemplate(TState &S)
//{
//   S.Update("Template");
//
//   switch (S.State) {
//      case 0 : {  //
//         if (S.NewState) {
//            Driver.XY(S.Param1, 0, S.Param1, 0);  // X, Y, Speed, EndSpeed - alles in mm(/sec)
//         }
//
//         if (Driver.IsDone()) { // Als de beweging klaar is
//            S.State++; // naar volgende state
//         }
//      }
//      break;
//
//      default : return true;  // error => mission end
//   }
//   return false;  // mission nog niet gereed
//}

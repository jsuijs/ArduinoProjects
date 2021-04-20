//-----------------------------------------------------------------------------
// Drive.cpp
//-----------------------------------------------------------------------------
//
// Per 'bewegingstype' heeft in deze class 2 methodes (functies). Neem
// bijvoorbeeld de beweging 'SpeedLR' (M_SPEED_LR), waarme je de gewenste
// snelheid van de linkse en rechtse motor kunt aangeven.
// De eerste methode is
//
//    void SpeedLR(int SpeedL, int SpeedR);
//
// Een aanroep naar deze public methode wordt een eventuele actieve beweging
// afgebroken en deze beweging wordt gestart. Of eigenlijk: alle parameters
// worden ingesteld voor deze beweging.
// De beweging zelf wordt uitgevoerd door regelmatig de methode Takt()
// aan te roepen. Deze kiest, op basis van de ingestelde parameters, de
// juiste uitvoeringsroutine. In dit geval:
//
//    void SpeedLRTakt(bool NewMovement, int SpeedL, int SpeedR, int MaxSlopeP);
//
// Dit is een private methode, die je dus niet zelf aanroept. Dat doet de Takt()
// methode. Maar... hier gebeurt het echte werk, op basis van de parameters die
// je via SpeedLR() hebt ingesteld.
//
// Tot slot de methode
//
//    bool IsDone();
//
// Deze methode geeft aan als een beweging is afgerond. Bijvoorbeeld bij de
// beweging 'XY', die de robot naar een bepaald punt laat rijden, geeft deze
// methode 'true' terug zodra dit punt bereikt is.
// Een beweging als 'SpeedLR' kent geen eindpunt. De methode IsDone() geeft
// dan nooit 'true' terug. In dit geval bepaal je op een andere manier wanneer
// je een volgende beweging start.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TDrive - constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TDrive::TDrive()
   {
      // call init when CSerial is up.
   }

//-----------------------------------------------------------------------------
// TDrive::init - manual called constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TDrive::init()
   {
      // reset : PWM mode, output 0.
      Pwm(0, 0);
      MaxSlope = MAX_SLOPE;
   }

//-----------------------------------------------------------------------------
// Takt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TDrive::Takt()
   {  static bool PrevIsDone = false;
      bool FirstCall = false;

      if (NewMovement) {
         // gewijzigde drive mode => diverse init's
         CSerial.printf("Drive.NewMovement => misc inits\n");
         FirstCall = true;

         SollSpeedL = ACT_SPEED_MM_SEC(Position.ActSpeedL);
         SollSpeedR = ACT_SPEED_MM_SEC(Position.ActSpeedR);
         NewMovement = false;
         IsDoneFlag  = false;
      }

      if (IsDoneFlag) {
         // Done en geen nieuwe beweging
         if (PrevIsDone == false) Motors(0,0);  // call once
         PrevIsDone = true;
         return;
      }
      PrevIsDone = false;

      //CSerial.printf("Drive.Takt %d %d %d\n", DriveMode, Param1, Param2);
      switch(DriveMode) {
         case M_PWM : {
            Motors(Param1, Param2);
            break;
         }
         case M_SPEED_LR : {
            SpeedLRTakt(FirstCall, Param1, Param2, MaxSlope);
            break;
         }
         case M_SPEED_HEADING : {
            SpeedHeadingTakt(FirstCall, Param1, Param2);
            break;
         }
         case M_XY : {
            IsDoneFlag = XYTakt(FirstCall, Param1, Param2, Param3, Param4);   // New, X, Y, Speed, EndSpeed
            break;
         }
         case M_ROTATE : {
            IsDoneFlag = RotateTakt(FirstCall, Param1);
            break;
         }
         case M_ARC : {
            IsDoneFlag = ArcTakt(FirstCall, Param1, Param2, Param3, Param4);
            break;
         }
         case M_STOP : {
            IsDoneFlag = StopTakt(FirstCall);
            break;
         }
         default :{
            CSerial.printf("Drive.Takt(): onbekende drivemode (%d)\n", DriveMode);
            TDrive();   // reset class
            break;
         }
      }
   }

//-----------------------------------------------------------------------------
// IsDone - return true als beweging klaar is.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TDrive::IsDone()
   {
      return IsDoneFlag;
   }

//-----------------------------------------------------------------------------
// TDrive::Pwm - rij met gegeven pwm waarden (L, R)
//-----------------------------------------------------------------------------
// Pwm* is pwm waarde + rijrichting, range 255...-255
//-----------------------------------------------------------------------------
void TDrive::Pwm(int PwmL, int PwmR)
   {
      if (DriveMode != M_PWM) {
         CSerial.printf("Drive.Pwm\n");
         NewMovement = true;
      }

      DriveMode = M_PWM;
      Param1 = PwmL;
      Param2 = PwmR;

      IsDoneFlag = false;
   }

//-----------------------------------------------------------------------------
// SpeedLR - rij met gegeven snelheid (L, R)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TDrive::SpeedLR(int SpeedL, int SpeedR)
   {
      if (DriveMode != M_SPEED_LR) {
        CSerial.printf("Drive.SpeedLR\n");
        NewMovement = true;
      }

      DriveMode = M_SPEED_LR;
      Param1 = SpeedL;
      Param2 = SpeedR;

      IsDoneFlag = false;
   }

//-----------------------------------------------------------------------------
// SpeedHeading - rij met gegeven snelheid in gegeven richting
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TDrive::SpeedHeading(int Speed, int Heading)
   {
      CSerial.printf("Drive.SpeedHeading\n");

      DriveMode = M_SPEED_HEADING;
      Param1 = Speed;
      Param2 = Heading;

      IsDoneFlag = false;
   }

//-----------------------------------------------------------------------------
// XY - Rij naar gegeven punt
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TDrive::XY(int X, int Y, int Speed, int EndSpeed)
   {
      CSerial.printf("Drive.XY  x: %d, y: %d, Speed: %d, EndSpeed: %d\n",
            X, Y, Speed, EndSpeed);

      DriveMode = M_XY;
      Param1 = X;
      Param2 = Y;
      Param3 = Speed;
      Param4 = EndSpeed;

      NewMovement = true;
      IsDoneFlag = false;
   }

//-----------------------------------------------------------------------------
// Rotate - draai naar absolute heading (graden).
//-----------------------------------------------------------------------------
// Draai de stilstaande robot naar opgegeven richting.
// - 'Heading' is de absolute hoek, in graden.
//
// Gebruikte constantes: ROTATE_P_GAIN, ROTATE_D_GAIN, ROTATE_CLIP
//-----------------------------------------------------------------------------
void TDrive::Rotate(int Heading)
   {
      CSerial.printf("Drive.Rotate\n");

      DriveMode = M_ROTATE;
      Param1 = NormHoek(Position.Hoek + Heading, 360); // Aantal te draaien graden (relatief).

      NewMovement = true;
      IsDoneFlag = false;
   }

//-----------------------------------------------------------------------------
// Rotate - draai naar absolute heading (graden).
//-----------------------------------------------------------------------------
// Draai de stilstaande robot naar opgegeven richting.
// - 'Degrees' is het aantal graden, positief = tegen de klok in. Waarde mag
//    groter zijn dan +/- 360 graden, de robot draait dan meer dan een hele ronde.
//
// Gebruikte constantes: ROTATE_P_GAIN, ROTATE_D_GAIN, ROTATE_CLIP
//-----------------------------------------------------------------------------
void TDrive::RotateRel(int Degrees)
   {
      CSerial.printf("Drive.RotateRel\n");

      DriveMode = M_ROTATE;
      Param1 = Degrees; // Aantal te draaien graden (relatief).

      NewMovement = true;
      IsDoneFlag = false;
   }

//-----------------------------------------------------------------------------
// Stop - breng de robot tot stilstand.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TDrive::Stop()
   {
      CSerial.printf("Drive.Stop\n");

      DriveMode = M_STOP;

      NewMovement = true;
      IsDoneFlag = false;
   }

//-----------------------------------------------------------------------------
// Arc - Rij boog naar heading (graden).
//-----------------------------------------------------------------------------
// Rij een boog tot de opgegeven richting.
// - 'Heading' is de absolute hoek, in graden.
// - Radius is straal van de draaicircel in mm
//-----------------------------------------------------------------------------
void TDrive::Arc(int Heading, int Radius, int Speed, int EndSpeed)
   {
      CSerial.printf("Drive.Arc\n");

      DriveMode = M_ARC;
      Param1 = Heading;
      Param2 = Radius;
      Param3 = Speed;
      Param4 = EndSpeed;

      NewMovement = true;
      IsDoneFlag = false;
   }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Hieronder de PRIVATE procedures
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SpeedLRTakt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TDrive::SpeedLRTakt(bool FirstCall, int SpeedL, int SpeedR, int MaxSlopeP)
   {
      CSerial.printf("SpeedLRTakt %d %d %d %d\n", FirstCall, SpeedL, SpeedR, MaxSlopeP);
      UpdateSpeedSP(SpeedL, SpeedR, MaxSlopeP);
      MotorController(SollSpeedL, SollSpeedR);
   }

//-----------------------------------------------------------------------------
// UpdateSpeedSP - werk Speed Setpoint (SollSpeed) bij
//-----------------------------------------------------------------------------
// Setpoint wordt gelijk aan parameters. Slope bepaalt hoe snel dit gebeurt.
//-----------------------------------------------------------------------------
void TDrive::UpdateSpeedSP(int InSpeedL, int InSpeedR, int MaxSlopeP)
   {
      Slope(SollSpeedL, InSpeedL, MaxSlopeP);
      Slope(SollSpeedR, InSpeedR, MaxSlopeP);
      CSerial.printf("@@ %d %d %d\n", SollSpeedL, InSpeedL, MaxSlopeP);
   }

//-----------------------------------------------------------------------------
// RotateTakt -
//-----------------------------------------------------------------------------
// Parameter: InDegrees - gewenste aantal graden draaien
// return: true when done
//-----------------------------------------------------------------------------
bool TDrive::RotateTakt(bool FirstCall, int InDegrees)
   {  static int RestHoek_q8, PrevRestHoek_q8;
      static int VorigeHoek_q8;
      static char StilStand;

      if (FirstCall) {
         RestHoek_q8    = InDegrees * 256;      // doel
         VorigeHoek_q8  = Position.HoekHires(); // start
         StilStand      = 0;                    // tbv eind-detectie
      }

      int DezeHoek_q8   = Position.HoekHires();
      int Delta_q8      = NormHoek(DezeHoek_q8 - VorigeHoek_q8, (360*256));
      RestHoek_q8      -= Delta_q8;
      VorigeHoek_q8     = DezeHoek_q8;

      int Clipped_q8 = Clip(RestHoek_q8, ROTATE_CLIP_Q8, -ROTATE_CLIP_Q8);
      int SpeedR = Clipped_q8 * (float) ROTATE_P_GAIN + (RestHoek_q8 - PrevRestHoek_q8) * (float) ROTATE_D_GAIN;
      int SpeedL = -SpeedR;

      CSerial.printf("RotateTakt FirstCall: %d InDegrees: %d DezeHoek: %d, RestHoek: %d, SpeedL: %d, Clipped: %d, Delta: %d\n",
            FirstCall, InDegrees, DezeHoek_q8/256, RestHoek_q8/256, SpeedL, Clipped_q8/256, Delta_q8/256);

      if (ABS(RestHoek_q8) > ABS(Clipped_q8)) {
         // clipped
         SpeedLRTakt(FirstCall, SpeedL, SpeedR, MAX_SLOPE);
      } else {
         // Niet geclipped, dus we zijn er blijkbaar bijna => vertraging in SpeedLR uitschakelen.
         SpeedLRTakt(FirstCall, SpeedL, SpeedR, MAX_SLOPE * 99);
      }

      if (RestHoek_q8 == PrevRestHoek_q8) {
         // stilstand
         StilStand ++;
         if (StilStand > 10) {
            return true; // klaar als we 10 ticks niet bewogen hebben.
         }
      } else {
         StilStand = 0;
      }

      PrevRestHoek_q8 = RestHoek_q8;
      return false; // not done yet
   }

//-----------------------------------------------------------------------------
// SpeedHeadingTakt -
//-----------------------------------------------------------------------------
// InSpeed in mm/sec
// InHeading in graden
//
// De snelheidswijziging verloopt via een slope
//-----------------------------------------------------------------------------
bool TDrive::SpeedHeadingTakt(bool FirstCall, int InSpeed, int InHeading)
   {  int SetSpeedL, SetSpeedR;
      static int SpeedSp = 0;

      // slope input speed
      if (FirstCall) {
         int l, r;
         //      l = ((ActSpeedL * ODO_TICK_TO_METRIC) ) / (ODO_TICK_HIRES_FACTOR * 4 * MAIN_TAKT_INTERVAL);
         //      r = ((ActSpeedR * ODO_TICK_TO_METRIC) ) / (ODO_TICK_HIRES_FACTOR * 4 * MAIN_TAKT_INTERVAL);
         l = ACT_SPEED_MM_SEC(Position.ActSpeedL);
         r = ACT_SPEED_MM_SEC(Position.ActSpeedR);
         SpeedSp = (l+r)/2;
         CSerial.printf("SpeedHeadingTakt FirstCall SpeedL: %d/%d SpeedR: %d/%d (mm/sec / raw), SpeedSp: %d, InHeading: %d\n", l, Position.ActSpeedL, r, Position.ActSpeedR, SpeedSp, InHeading);
      } else {
         Slope(SpeedSp, InSpeed, MAX_SLOPE);
//         CSerial.printf("SpeedHeadingTakt InSpeed: %d, SpeedSp: %d, InHeading: %d\n", InSpeed, SpeedSp, InHeading);
      }

      // Hoekfout bepalen (i.c.m. richting)
      long CurrentHoek = Position.HoekHires();
      long HoekError   = NormHoek(CurrentHoek - InHeading * 256L, (360L * 256));

      // Eigenlijk hoort SpeedSp niet (langere tijd) 0 te zijn.
      // Als deze toch 0 is en de hoekerror is klein, dan onderdrukt
      // de volgende regel het corrigeren van een kleine hoekfout.
      if ((SpeedSp == 0) && (ABS(HoekError) < 256)) {
         HoekError = 0;
      }

      // simpele P regelaar voor richting  (zie ook UmArcTakt)
      long Correctie = (HoekError * PID_Kp) / 4096;

      long ClippedCorrectie = Clip(Correctie, PID_OUT_CLIP * 256L, PID_OUT_CLIP * -256L);

      SetSpeedL = SpeedSp + (ClippedCorrectie * WIEL_BASIS) / (1024);
      SetSpeedR = SpeedSp - (ClippedCorrectie * WIEL_BASIS) / (1024);

//      CSerial.printf("%ld %ld %ld %ld %d %d\n",
//            CurrentHoek, HoekError, Correctie, ClippedCorrectie, SetSpeedL, SetSpeedR);

      SpeedLRTakt(FirstCall, SetSpeedL, SetSpeedR, MAX_SLOPE);

      return false; // never done
   }

//-----------------------------------------------------------------------------
// XYTakt - Rij naar punt (X, Y)
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool TDrive::XYTakt(bool FirstCall, int TargetX, int TargetY, int Speed, int EndSpeed)
   {  static long SaveHeading; // rijrichting (voor de laatste paar cm)
      static int LastTargetDistance;
      static int State = 0;
      long TargetHeading;
      int  TargetDistance;

      if (FirstCall) {
         CSerial.printf ("XYTakt x: %d, y: %d, Speed: %d, EndSpeed: %d\n", TargetX, TargetY, Speed, EndSpeed);
         State = 0;
      }


      // doel bepalen (steeds opnieuw).
      // hoek & afstand bepalen, resultaat in graden*256 (360/circel) en mm
      Cartesian2Polar(TargetHeading, TargetDistance, TargetX - Position.XPos, TargetY - Position.YPos);
      CSerial.printf ("XYTakt TargetVector %d mm, %d graden (%d %d %d %d))\n", TargetDistance, (int)(TargetHeading/256),
            TargetX, Position.XPos, TargetY, Position.YPos);

      //   CSerial.printf("run plaats: ( %d, %d ), doel: ( %d, %d ), TargetDistance: %d\n", RobotXPos()/10, RobotYPos()/10, TargetX, TargetY, TargetDistance);

      if (TargetDistance < 5) {
         CSerial.printf("XYTakt done < 5mm\n");
         return true; // stap is afgerond als we minder dan 10mm afstand tot doel hebben
      }

      switch(State) {
         case 0: {   // eerste itteratie: wel afstand bepalen & bewaren, maar geen conclusie mogelijk.
            LastTargetDistance = TargetDistance;
            SaveHeading = TargetHeading;
            State ++;
            break;
         }
         case 1: {   // wacht op afstands-afname van meer dan 70 mm (nodig in geval we moeten draaien)
            if ((TargetDistance + 70) < LastTargetDistance) {
               LastTargetDistance = TargetDistance;
               State ++;
            }
            break;
         }
         case 2: {   // wacht tot we het doel bereikt hebben
            // afstands-toename van meer dan 2 cm
            if (TargetDistance > (LastTargetDistance + 20)) {
//               CSerial.printf("XYTakt done increment %d %d", TargetDistance, LastTargetDistance);

               return 1; // okay => stap is afgerond
            }
            // LastTargetDistance bevat kleinste afstand tot nu toe
            if (LastTargetDistance > TargetDistance) {
               LastTargetDistance = TargetDistance;
            }
            break;
         }
      }

      // op korte afstand, Heading gebruiken in plaats van echte richting.
      // (dit om te voorkomen dat de robot een slinger maakt vlak bij of voorbij het punt)
      long HeadingOut;
      if (TargetDistance > 50) {
         // grote afstand => TargetHeading gebruiken
         HeadingOut = TargetHeading; // graden*256
         SaveHeading = TargetHeading;
      } else {
         // we zijn op minder dan 50mm afstand
         HeadingOut = SaveHeading;
      }

      if (Speed < 0) {
         // negative speed => heading +180
         HeadingOut = NormHoek(HeadingOut + 180*256L, 360*256L);
      }

      Speed = EenparigVertragen(TargetDistance, Speed, EndSpeed, MAX_SLOPE * MAIN_TAKT_RATE*2);

      SpeedHeadingTakt(FirstCall, Speed, HeadingOut/256); // Bij FirstCall neem SpeedHeadingTakt de huidige snelheid als startpunt

//      CSerial.printf("XYTakt State: %d, HeadingOut: %d, distance: %d, SpeedOut: %d\n", State, (int)(HeadingOut / 256), TargetDistance, Speed);

      return false; // not done yet
   }

//-----------------------------------------------------------------------------
// ArcTakt - Rij boog naar heading
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool TDrive::ArcTakt(bool FirstCall, int Heading, int Radius, int Speed, int EndSpeed)
   {  static int   DoelOdoT;
      static float TurnRate;

      int OdoL, OdoR, OdoT;  // afgelegde weg in mm
      Position.OdoGet(OdoL, OdoR, OdoT);

      if (FirstCall) {
         // bepaal
         // - hoeveel we moeten rijden tot het eindpunt van de boog (DoelOdoT)
         // - hoeveel graden we moeten draaien per afgelegde mm (Turnrate)
         int DeltaHoek = NormHoek(Heading - Position.Hoek, 360);
         DoelOdoT = OdoT + 2 * 3.14159 * Radius  * ABS(DeltaHoek) / 360;
         TurnRate = 360 / 2 / 3.14159 / Radius; // TurnRate in graden / mm
         if (DeltaHoek < 0) {
            TurnRate = -TurnRate;
         }
         CSerial.printf("ArcTakt First DoelOdoT: %d, TurnRate x 100: %d (%d %d %d %d)\n", DoelOdoT, (int)(TurnRate * 100), Heading, Radius, OdoT, Position.Hoek);
      }

      int RestantWeg = DoelOdoT - OdoT;
      if (RestantWeg < 10)  {
         return true;   // done
      }

      long CurrentHoek = Position.HoekHires(); // (graden *256)
      int  TargetHoek  = Heading - TurnRate * RestantWeg;  // deze hoek willen we nu hebben (graden).
      long HoekError   = NormHoek(CurrentHoek - TargetHoek * 256L, (360L * 256));

      // bepaal maximale snelheid op gegeven afstand van doel, en verschil tussen L en R o.b.v. radius
      int SpeedL = EenparigVertragen(RestantWeg, Speed, EndSpeed, MAX_SLOPE * MAIN_TAKT_RATE); // max speed
      int SpeedR = SpeedL;
      {  // limit scope Delta
         int Delta = SpeedL * (WIEL_BASIS / 2) / Radius;
         SpeedL += Delta;
         SpeedR -= Delta;
      }

      // simpele P regelaar voor richting (zie ook UmSpeedHeadingTakt)
      {  // limit scope Correctie
         int Correctie = (HoekError * PID_Kp) / 4096;
         int ClippedCorrectie = Clip(Correctie, PID_OUT_CLIP * 256, PID_OUT_CLIP * -256);
         SpeedL = SpeedL + (ClippedCorrectie * WIEL_BASIS) / (1024);
         SpeedR = SpeedR - (ClippedCorrectie * WIEL_BASIS) / (1024);
      }

      // stuur de motoren
      SpeedLRTakt(FirstCall, SpeedL, SpeedR, MAX_SLOPE);

      CSerial.printf("ArcTakt FirstCall: %d Heading: %d Radius: %d, RestantWeg: %d, TargetHoek: %d, CurHoek: %d, HoekError: %d, SpeedL: %d, SpeedR: %d\n",
            FirstCall, Heading, Radius, RestantWeg, TargetHoek, (int)(CurrentHoek/256), (int)(HoekError/256), SpeedL, SpeedR);

      return false; // not done yet
   }

//-----------------------------------------------------------------------------
// StopTakt -
//-----------------------------------------------------------------------------
// De snelheidswijziging verloopt via een slope
//-----------------------------------------------------------------------------
bool TDrive::StopTakt(bool FirstCall)
   {  static int SavedHeading = 0;

      // save heading
      if (FirstCall) {
         SavedHeading = Position.Hoek;
      }

      SpeedHeadingTakt(FirstCall, 0, SavedHeading);

      if ((SollSpeedL + SollSpeedR) != 0) return false;  // we staan nog niet stil.

      return true;   // done (we staan stil, of toch bijna, de beide setpoints zijn samen 0.

   }

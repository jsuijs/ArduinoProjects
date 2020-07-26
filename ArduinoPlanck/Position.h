//-----------------------------------------------------------------------------
// Position.h
//-----------------------------------------------------------------------------
// Let op: deze file heeft als extensie .h, maar bevat ook code en is eigenlijk
//         een combinatie van een .h en een .cpp file. Dit is (lijkt) de
//         enige manier die voldoende controle geeft om een class in een Arduino
//         project op te nemen. Met include bepaal je zelf waar de class wordt
//         ingevoegd.
//-----------------------------------------------------------------------------

#include "math.h"
#include "stdio.h"

#define TICKS_360_GRADEN (360L * 256 * 256 / ODO_HEADING)
#define GRAD2RAD(x) ((float)(x) / 57.2957795)

class TPosition
{
   public:

      TPosition();
      void Takt();
      void Reset();

      void OdoGet(int *OdoL_out, int *OdoR_out, int *OdoT_out) ;
      void Print();

      int  ActSpeedL, ActSpeedR;    // in odo_ticks per MAIN_TAKT_INTERVAL
      int  XPos;  // in mm
      int  YPos;
      int  Hoek;  // in graden

      long HoekHires();

   private:

      // private vars

      // de robot positie.
      long int VarRobotXPos;    // in 1/1024 mm (ca 1 um)
      long int VarRobotYPos;    // in 1/1024 mm (ca 1 um)
      long int VarRobotHoek;    // in 360*256 /circel (graden*256)

      long int OdoL_ticks;      // afstand in odo_ticks  - LET OP - alleen voor hoek; var maakt sprong van '360 graden' ticks.
      long int OdoR_ticks;      // afstand in odo_ticks  - LET OP - alleen voor hoek;

      long int OdoL;            // afstand in mm * 1024 (ongeveer um)
      long int OdoR;            // afstand in mm * 1024 (ongeveer um)
      long int OdoT;            // afstand in mm * 1024 (ongeveer um) (gemiddelde van L+R, absolute waarde!)

      // private methods
      void Update();
};

//-----------------------------------------------------------------------------
// TPosition - constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TPosition::TPosition()
   {
      Reset();
   }

//-----------------------------------------------------------------------------
// OdoGet - Get OdoL, R (in mm), OdoT (absolute waarde, in mm)
//-----------------------------------------------------------------------------
// Gebruik deze routine voor besturing waarbij de x/y/hoek niet
// bruikbaar (of handig) is.
//-----------------------------------------------------------------------------
void TPosition::OdoGet(int *OdoL_out, int *OdoR_out, int *OdoT_out)
   {
      *OdoL_out = OdoL / 1024;
      *OdoR_out = OdoR / 1024;
      *OdoT_out = OdoT / 1024;
   }

//------------------------------------------------------------------------
// ResetRobotPosition - Zet huidige positie op 0, 0, 0
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void TPosition::Reset()
   {
      printf("ResetRobotPosition\n");
      VarRobotXPos = 0;
      VarRobotYPos = 0;
      VarRobotHoek = 0;

      // odo hulp vars:
      OdoL_ticks = 0;
      OdoR_ticks = 0;
      OdoT  = 0;
      OdoL  = 0;
      OdoR  = 0;

      Update();   // update variabelen
   }


long TPosition::HoekHires()
   {
      return VarRobotHoek;
   }


//-----------------------------------------------------------------------------
// PrintRobotPositie -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TPosition::Print()
   {
      Update();
      printf("RobotPosition X: %d, Y: %d, Hoek: %d, ActSpeed %d / %d\n", XPos, YPos, Hoek,ActSpeedL, ActSpeedR);
   }

//-----------------------------------------------------------------------------
// OdoTakt - Lees encoders & werk RobotPositie bij
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TPosition::Takt()
   {	float RadHoek;
      long  DeltaT;
      static long OdoTickLR_Correction = 0;

      // Haal encoder gegevens op
      EncoderRead (ActSpeedL, ActSpeedR);

      // Corrigeer voor verschil in wiel-grootte
      OdoTickLR_Correction += ActSpeedR * (long)ODO_TICK_L_R;
      ActSpeedR = OdoTickLR_Correction / 4096;
      OdoTickLR_Correction -= ActSpeedR * 4096L;

      // Hou ticks bij, om richting te bepalen.
      OdoL_ticks += ActSpeedL;
      OdoR_ticks += ActSpeedR;

      // corrigeer OdoL_ticks zodat de delta niet te hoog oploopt.
      // (Als de delta te groot wordt, kan een wrap-around optreden bij berekening van VarRobotHoek)
      // (gekozen voor +/- 360 graden (en niet +/- 180) zodat er hysteresis tussen de correctie-punten zit)
      long d = OdoR_ticks - OdoL_ticks;
      if (d > TICKS_360_GRADEN) {
         OdoL_ticks += TICKS_360_GRADEN;
         printf("OdoL_ticks jump up %ld\n", OdoL_ticks);
      }

      if (d < -TICKS_360_GRADEN) {
         OdoL_ticks -= TICKS_360_GRADEN;
         printf("OdoL_ticks jump down %ld\n", OdoL_ticks);
      }

      //   printf("ActSpeedL: %d, ActSpeedR: %d, OdoL_ticks: %ld, OdoR_ticks: %ld\n",
      //      ActSpeedL, ActSpeedR, OdoL_ticks, OdoR_ticks);

      if ((ActSpeedL !=0) || (ActSpeedR != 0)) { // als we verplaatst zijn

         // reken afgelegde weg om naar micrometers (odo interne eenheid)
         long DeltaL, DeltaR;
         DeltaL = ((ActSpeedL * ODO_TICK_TO_METRIC) + 2) / 4 ;   // +2 voor afronding, /4 omdat ODO_TICK_TO_METRIC *4096 is, en DeltaL in mm*1024
         DeltaR = ((ActSpeedR * ODO_TICK_TO_METRIC) + 2) / 4 ;
         DeltaT = (DeltaL + DeltaR) / 2; 						// /2 (ivm sum L+R)

         if (DeltaT > 0) { 	// Absolute waarde / totaal afgelegde weg (o.a. voor compas)
            OdoT += DeltaT ;
         } else {
            OdoT -= DeltaT ;
         }
         OdoL += DeltaL;
         OdoR += DeltaR;

         // werk de absolute robot-positie bij (0 graden = rijrichting x, positieve hoek = draai naar rechts)
         RadHoek = GRAD2RAD(VarRobotHoek) / 256;
         // update X/Y positie
         long tmp;
         tmp = 0.5 + DeltaT * cos(RadHoek);
         VarRobotXPos += tmp;
         tmp = 0.5 + DeltaT * sin(RadHoek);
         VarRobotYPos += tmp;

         Update();
//        printf("SpeedL: %d, SpeedR: %d, Lticks: %ld, Rticks: %ld, DeltaL: %ld, DeltaR: %ld, XPos: %d YPos: %d Hoek: %d\n",
//	      ActSpeedL, ActSpeedR, OdoL_ticks, OdoR_ticks, DeltaL, DeltaR, XPos, YPos, Hoek);
      }
   }


//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Hieronder de PRIVATE procedures
//------------------------------------------------------------------------
//------------------------------------------------------------------------


#define NORM  (360L * 256)
//------------------------------------------------------------------------
// Update - update public var's
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void TPosition::Update()
   {
      //--------------------
      // Bereken nieuwe hoek
      VarRobotHoek = ((OdoR_ticks - OdoL_ticks) * ODO_HEADING) / 256;  // heading in graden*256

      // Normaliseer hoek
      VarRobotHoek %= NORM;  // +/- 360*256 graden
      while (VarRobotHoek > (NORM/2))   VarRobotHoek -= NORM;  // > 180 graden (als norm=360)
      while (VarRobotHoek <= (NORM/-2)) VarRobotHoek += NORM;   // =< -180 graden (als norm=360) (<=, niet < omdat anders zowel 180 als -180 geldig zouden zijn)

      //--------------------
      // update public var's
      XPos  = VarRobotXPos / 1024;  // RobotXPos in mm
      YPos  = VarRobotYPos / 1024;  // RobotYPos in mm
      Hoek  = VarRobotHoek / 256;   // RobotHoekPos in graden
   }

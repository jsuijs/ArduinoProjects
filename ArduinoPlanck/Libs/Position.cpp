//-----------------------------------------------------------------------------
// Position.cpp
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "math.h"
#include "stdio.h"

//-----------------------------------------------------------------------------
// TPosition - constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TPosition::TPosition()
   {
      // call init when CSerial is up.
   }

//-----------------------------------------------------------------------------
// OdoGet - Get OdoL, R (in mm), OdoT (absolute waarde, in mm)
//-----------------------------------------------------------------------------
// Gebruik deze routine voor besturing waarbij de x/y/hoek niet
// bruikbaar (of handig) is.
//-----------------------------------------------------------------------------
void TPosition::OdoGet(int &OdoL_out, int &OdoR_out, int &OdoT_out)
   {
      OdoL_out = OdoL / 1024;
      OdoR_out = OdoR / 1024;
      OdoT_out = OdoT / 1024;
   }

//------------------------------------------------------------------------
// ResetRobotPosition - Zet huidige positie op 0, 0, 0
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void TPosition::Reset()
   {
      CSerial.printf("ResetRobotPosition\n");
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

//-----------------------------------------------------------------------------
// PrintRobotPositie -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TPosition::Print()
   {
      Update();
      CSerial.printf("RobotPosition X: %d, Y: %d, Hoek: %d, ActSpeed %d / %d\n", XPos, YPos, Hoek,ActSpeedL, ActSpeedR);
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
      ReadStmEncodersDelta(ActSpeedL, ActSpeedR);

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
         CSerial.printf("OdoL_ticks jump up %ld\n", OdoL_ticks);
      }

      if (d < -TICKS_360_GRADEN) {
         OdoL_ticks -= TICKS_360_GRADEN;
         CSerial.printf("OdoL_ticks jump down %ld\n", OdoL_ticks);
      }

      //   CSerial.printf("ActSpeedL: %d, ActSpeedR: %d, OdoL_ticks: %ld, OdoR_ticks: %ld\n",
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
         tmp = DeltaT * cos(RadHoek);
         VarRobotXPos += tmp;
         tmp = DeltaT * sin(RadHoek);
         VarRobotYPos += tmp;

         Update();
//       CSerial.printf("SpeedL: %d, SpeedR: %d, Lticks: %ld, Rticks: %ld, DeltaL: %ld, DeltaR: %ld, XPos: %d YPos: %d Hoek: %d\n",
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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Encoders.ino - Dit bestand bevat de low level routines voor de encoder.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define MAQUEENPLUS_PIN_ENCODER_L_A    PA6   //PA6
#define MAQUEENPLUS_PIN_ENCODER_L_B    PA7   //PA7
#define MAQUEENPLUS_TIMER_ENCL         TIM3  // 16 bits timer/counter

#define MAQUEENPLUS_PIN_ENCODER_R_A    PA1    //PA0
#define MAQUEENPLUS_PIN_ENCODER_R_B    PA0    //PA1
#define MAQUEENPLUS_TIMER_ENCR         TIM2  // 16 bits timer/counter

HardwareTimer TimerEncL(MAQUEENPLUS_TIMER_ENCL);
HardwareTimer TimerEncR(MAQUEENPLUS_TIMER_ENCR);

void InitStmEncoders()
{
   //-----------------------------------------------------------------------
   // Init hardware quadrature encoders & corresponding IO
   //-----------------------------------------------------------------------
   pinMode(MAQUEENPLUS_PIN_ENCODER_L_A, INPUT_PULLUP);
   pinMode(MAQUEENPLUS_PIN_ENCODER_L_B, INPUT_PULLUP);
   pinMode(MAQUEENPLUS_PIN_ENCODER_R_A, INPUT_PULLUP);
   pinMode(MAQUEENPLUS_PIN_ENCODER_R_B, INPUT_PULLUP);

   // setup hardware quadrature encoders (requires STM32Cube functions)
   TIM_HandleTypeDef    Encoder_Handle;
   TIM_Encoder_InitTypeDef sEncoderConfig;

   /* Initialize TIM* peripheral as follow:
       + Period = 65535
       + Prescaler = 0
       + ClockDivision = 0
       + Counter direction = Up

      https://community.st.com/s/question/0D50X00009XkfbN/quadrature-encoder
      STM32Cube_FW_F4_V1.16.0\Projects\STM32469I_EVAL\Examples\TIM\TIM_Encoder\Src\main.c
   */

   Encoder_Handle.Init.Period             = 65535;
   Encoder_Handle.Init.Prescaler          = 0;
   Encoder_Handle.Init.ClockDivision      = 0;
   Encoder_Handle.Init.CounterMode        = TIM_COUNTERMODE_UP;
   Encoder_Handle.Init.RepetitionCounter  = 0;
   Encoder_Handle.Init.AutoReloadPreload  = TIM_AUTORELOAD_PRELOAD_DISABLE;

   sEncoderConfig.EncoderMode             = TIM_ENCODERMODE_TI12;

   sEncoderConfig.IC1Polarity             = TIM_ICPOLARITY_RISING;
   sEncoderConfig.IC1Selection            = TIM_ICSELECTION_DIRECTTI;
   sEncoderConfig.IC1Prescaler            = TIM_ICPSC_DIV1;
   sEncoderConfig.IC1Filter               = 0;

   sEncoderConfig.IC2Polarity             = TIM_ICPOLARITY_RISING;
   sEncoderConfig.IC2Selection            = TIM_ICSELECTION_DIRECTTI;
   sEncoderConfig.IC2Prescaler            = TIM_ICPSC_DIV1;
   sEncoderConfig.IC2Filter               = 0;

   Encoder_Handle.Instance = MAQUEENPLUS_TIMER_ENCL;
   if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK) CSerial.println("TIM2 init error");
   HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);

   Encoder_Handle.Instance = MAQUEENPLUS_TIMER_ENCR;
   if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK) CSerial.println("TIM3 init error");
   HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);

//   systick_attach_callback(&encoder1_read);
}

void ReadStmEncodersDelta(int &Left, int &Right)
   {  static int RawEncoderLeft, RawEncoderRight;

      // save prev values
      int PrevEncoderLeft  = RawEncoderLeft;
      int PrevEncoderRight = RawEncoderRight;

      // read raw
      RawEncoderLeft  = -TimerEncL.getCount(); // flip sign here if required
      RawEncoderRight = TimerEncR.getCount(); // flip sign here if required

      // difference (cast to short int required to properly handle wrap around of 16-bit counters)
      Left  = (short int)(RawEncoderLeft  - PrevEncoderLeft);
      Right = (short int)(RawEncoderRight - PrevEncoderRight);
   }

//-----------------------------------------------------------------------------
// Utilities.cpp
//-----------------------------------------------------------------------------
#include "math.h"

//----------------------------------------------------------------------------
// EenparigVertragen - Begrens snelheid, afhankelijk van afstand tot einde
//----------------------------------------------------------------------------
// Parameters:
//       Afstand     - Afstand tot eindpunt (in mm)
//       SetSpeed    - gewenste (maximale) snelheid (in mm/s)
//       EndSpeed    - gewenste snelheid op eindpunt (in mm/s)
//       Vertraging  - vertraging in mm/s^2
//
// return Speed (in mm/s), met teken van SetSpeed
//----------------------------------------------------------------------------
int EenparigVertragen( int Afstand, int SetSpeed, int EndSpeed, int Vertraging)
{
   if (ABS(EndSpeed) >= ABS(SetSpeed)) {
      // EndSpeed hoger => we hoeven niet in te houden.
      return SetSpeed;
   }

   //-----------------------------------------------------------------------
   // vertraag als we het doel naderen.
   // vmax = sqrt( 2 * a [cm/s2] * d [cm] + v_eind^2)
   // a = MAX_SLOPE [mm / s / MAIN_TAKT_INTERVAL]
   // (MAIN_TAKT_RATE = 1 / MAIN_TAKT_INTERVAL)
   // d = TargetDistance [mm]
   //  2 / 2.56 / 10 = 1 / 12,8
   // factor 2 om voldoende speelruimte te houden t.o.v. maximale speedstep
   // (norm 50% van de tijd een stap)
   //-----------------------------------------------------------------------

   int MaxSpeed = sqrt(Vertraging * (long) Afstand  / 2 +  EndSpeed * (long) EndSpeed);
   if (MaxSpeed < ABS(SetSpeed)) {
      // speed limited
      //CSerial.printf("EenparigVertragen %d %d %d %d %d\n", Afstand, SetSpeed, EndSpeed, Vertraging, MaxSpeed);

      if (SetSpeed > 0) {
         return  MaxSpeed; // speed limited
      } else {
         return -MaxSpeed; // speed limited
      }
   }
   return SetSpeed;  // ongewijzigde snelheid.
}

//----------------------------------------------------------------------------
// Clip - return input, begrensd bij (min, max)
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
long Clip(long input, long min, long max)
{	long tmp;

	if (max < min) {
		tmp = min; min = max; max = tmp;
	}

   if (input < min) return min;
   if (input > max) return max;
   return input;
}

//-----------------------------------------------------------------------------
// Slope - Gradually increment / decrement to Setpoint.
//-----------------------------------------------------------------------------
// Step is positive value.
// SlopeInOut follows setpoint
//-----------------------------------------------------------------------------
void Slope(int &SlopeInOut, int Setpoint, int Step)
{
   Step = ABS(Step);  // make sure Step is positive
   if (Setpoint > SlopeInOut) {
      // upward slope
      SlopeInOut += Step;
      if (SlopeInOut > Setpoint) SlopeInOut = Setpoint; // correct overshoot
   } else {
      // downward slope
      SlopeInOut -= Step;
      if (SlopeInOut < Setpoint) SlopeInOut = Setpoint; // correct overshoot
   }
   return;
}

//----------------------------------------------------------------------------
// NormHoek - Normeer een hoek rond nulpunt (+/- halve circel).
//----------------------------------------------------------------------------
// Param
//    hoek = input,
//    Norm = volle circel
//
// Return: normalized hoek
//
// Norm 360 geeft hoek tussen +180 en -180
//----------------------------------------------------------------------------
long NormHoek(long hoek, long Norm)
{  long Temp = Norm / 2;

   hoek %= Norm;  // +/- 360 graden (als norm=360)
   while (hoek > Temp)  hoek -= Norm;  // > 180 graden (als norm=360)
   Temp = -Temp;
   while (hoek <= Temp) hoek += Norm;   // =< -180 graden (als norm=360) (<=, niet < omdat anders zowel 180 als -180 geldig zouden zijn)
   return hoek;
}

//----------------------------------------------------------------------------
// Cartesian2Polar - van X/Y naar afstand/hoek
//----------------------------------------------------------------------------
// Hoek in graden*256
// Als pointer naar afstand NULL is, dan alleen X/Y -> hoek.
//----------------------------------------------------------------------------
void Cartesian2Polar(long &hoek, int &afstand, int x, int y)
{
//   float f = atan2((float) y, (float) x);
   hoek = (long)(256 * RAD2GRAD(atan2((float) y, (float) x)));   // inderdaad: y is 1e param, x = 2e

   afstand = (int) sqrt((long)x * (long)x + (long)y * (long)y);

//   CSerial.printf("Cartesian2Polar %d %d %d %d\n", (int)(hoek>>8), afstand, x, y);
}


//-----------------------------------------------------------------------------
// HalI2cClearBus -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void I2cClearBus(int PinSda, int PinScl)
{
   pinMode(PinSda, INPUT);
   pinMode(PinScl, OUTPUT);
//   InitGPIO(GPIOB, I2C_PIN_SCL, GPIO_Mode_OUT, GPIO_OType_OD);
//-    InitGPIO(GPIOB, I2C_PIN_SDA, GPIO_Mode_OUT, GPIO_OType_OD);

//   GPIO_SetBits(GPIOB, I2C_PIN_SDA);  // SDA high
//-   GPIO_SetBits(GPIOB, I2C_PIN_SCL);  // SCL high
   digitalWrite(PinScl, 1);
   delay(5);

   //toggle SCL 9+ times
   for (int i = 0; i< 10; i++) {
      digitalWrite(PinScl, 0);
      delay(1);
      digitalWrite(PinScl, 1);
      delay(1);
   }

//   GPIO_ResetBits(GPIOB, I2C_PIN_SCL);  // SCL low allows setup of SDA
   digitalWrite(PinScl, 0);
   delay(1);

//   GPIO_ResetBits(GPIOB, I2C_PIN_SDA);  // SDA setup (low)
   pinMode(PinSda, OUTPUT);
   digitalWrite(PinSda, 0);
   delay(1);

//   GPIO_SetBits(GPIOB, I2C_PIN_SCL);  // SCL high, prepare for STOP
   digitalWrite(PinScl, 1);
   delay(1);

//   GPIO_SetBits(GPIOB, I2C_PIN_SDA);  // SDA low->high while CLK high => STOP
   digitalWrite(PinSda, true);
   delay(1);

   pinMode(PinSda, INPUT);
   pinMode(PinScl, INPUT);
}

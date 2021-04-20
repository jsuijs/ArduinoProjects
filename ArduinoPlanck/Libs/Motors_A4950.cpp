//-----------------------------------------------------------------------------
// Motors_A4950.ino  - HL aansturing van de RedBot motors (hard, push-pull)
//                    A4950 chip direct aangesloten
//-----------------------------------------------------------------------------
#include "MyRobot.h"

//-----------------------------------------------------------------------------
// SetupMotors - Stel IO in voor aansturing motoren (harde aansturing)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MotorsSetup()
{
  // 4x PWM
  pinMode(A4950_PWML1, OUTPUT);
  pinMode(A4950_PWML2, OUTPUT);
  pinMode(A4950_PWMR1, OUTPUT);
  pinMode(A4950_PWMR2, OUTPUT);

  // zet output op 0 via Motors()
  Motors(0, 0);
}

//------------------------------------------------------------------------------
// Motors - harde aansturing
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Motors(int PwmL, int PwmR)
{
  //  CSerial.printf("Motors %d %d\n", PwmL, PwmR);
  //--------
  // Motor L
  //--------

  // begrens waarden
  int Pwm = constrain(PwmL, -255, 255);

   if (Pwm >=0) {
      analogWrite(A4950_PWML1, 255 - Pwm);
      analogWrite(A4950_PWML2, 255);
   } else {
      analogWrite(A4950_PWML1, 255);
      analogWrite(A4950_PWML2, Pwm);
   }

  //--------
  // Motor R
  //--------

  // begrens waarden
  Pwm = constrain(PwmR, -255, 255);

  // Set PWM en richting
   if (Pwm >=0) {
      analogWrite(A4950_PWMR1, 255 - Pwm);
      analogWrite(A4950_PWMR2, 255);
   } else {
      analogWrite(A4950_PWMR1, 255);
      analogWrite(A4950_PWMR2, Pwm);
   }
}

// motors.cpp
#include "project.h"

#define Pin_Motor_DirL PB0
#define Pin_Motor_DirR PB2
#define Pin_Motor_PwmL PA6
#define Pin_Motor_PwmR PA7

//-----------------------------------------------------------------------------
// MotorsInit -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MotorsInit()
{
   // pwm & dir pin setup
   pinMode(Pin_Motor_PwmL, OUTPUT);
   pinMode(Pin_Motor_PwmR, OUTPUT);
   pinMode(Pin_Motor_DirL, OUTPUT);
   pinMode(Pin_Motor_DirR, OUTPUT);

   Motors(0, 0);
}

//-----------------------------------------------------------------------------
// Motors -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Motors(int PwmL, int PwmR)
{
   digitalWrite(Pin_Motor_DirL, PwmL >= 0);
   PwmL = abs(PwmL);
   if (PwmL > 255) PwmL = 255;
   analogWrite(Pin_Motor_PwmL, PwmL);

   digitalWrite(Pin_Motor_DirR, PwmR >= 0);
   PwmR = abs(PwmR);
   if (PwmR > 255) PwmR = 255;
   analogWrite(Pin_Motor_PwmR, PwmR);
}

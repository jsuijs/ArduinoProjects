// motors.cpp
#include "project.h"


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


//Define Variables we'll be connecting to
double PidL_In, PidL_Out, PidL_Sp;
double PidR_In, PidR_Out, PidR_Sp;

//Specify the links and initial tuning parameters
double Kp = 2, Ki = 5, Kd = 1;
PID PidL(&PidL_In, &PidL_Out, &PidL_Sp, Kp, Ki, Kd, DIRECT);
PID PidR(&PidR_In, &PidR_Out, &PidR_Sp, Kp, Ki, Kd, DIRECT);

//-----------------------------------------------------------------------------
// PidTakt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void PidTakt(float SpL, float SpR)
{ static bool FirstCall = true;

  if (FirstCall) {
    // init Pid's - initialize the inputs & set PIDs to automatic
    PidL_In = PidR_In = 0;
    PidL_Sp = PidR_Sp = 0;
    
    PidL.SetMode(AUTOMATIC);
    PidR.SetMode(AUTOMATIC);

    FirstCall = false;
  } else {
    // Normal operation    
    PidL_Sp = SpL;
    PidR_Sp = SpR;

    PidL_In = 123;
    PidR_In = 123;
  }

  PidL.Compute();
  PidR.Compute();

  Motors(PidL_Out, PidR_Out);
}

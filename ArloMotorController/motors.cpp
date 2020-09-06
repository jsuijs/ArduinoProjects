// motors.cpp
#include "project.h"

#ifdef USE_PID
// Define PID variables we'll be connecting to
double PidL_In, PidL_Out, PidL_Sp;
double PidR_In, PidR_Out, PidR_Sp;

// Specify the links and initial tuning parameters
double Kp = 0.0, Ki = 3.0, Kd = 0.0;
PID PidL(&PidL_In, &PidL_Out, &PidL_Sp, Kp, Ki, Kd, P_ON_M, DIRECT);
PID PidR(&PidR_In, &PidR_Out, &PidR_Sp, Kp, Ki, Kd, P_ON_M, DIRECT);

#endif

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

#ifdef USE_PID
   // init Pid's - initialize the inputs & set PIDs to automatic
   PidL_In = PidR_In = 0;
   PidL_Sp = PidR_Sp = 0;

   SetPidSampleTime();

   PidL.SetMode(AUTOMATIC);
   PidR.SetMode(AUTOMATIC);
#endif

}

//-----------------------------------------------------------------------------
// SetPidSampleTime -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SetPidSampleTime()
{
#ifdef USE_PID
   PidL.SetSampleTime(CfgLoopTime);
   PidR.SetSampleTime(CfgLoopTime);
#endif
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

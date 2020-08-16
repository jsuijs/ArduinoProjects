// project.h
#include "Arduino.h"
#include "PID_v1.h"

#define Pin_Led  33

#define Pin_Motor_DirL PB2
#define Pin_Motor_DirR PB0
#define Pin_Motor_PwmL PA7
#define Pin_Motor_PwmR PA6

extern HardwareSerial Serial, Serial2;


void MotorsInit();
void Motors(int PwmL, int PwmR);

extern PID PidL, PidR;
extern double PidL_In, PidL_Out, PidL_Sp;
extern double PidR_In, PidR_Out, PidR_Sp;

void EncodersInit();
void EncodersRead (int &Left, int &Right);

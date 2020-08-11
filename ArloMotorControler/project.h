// project.h
#include "arduino.h"
#include "PID_v1.h"


#define Pin_Led  33

#define Pin_Motor_DirL PB0
#define Pin_Motor_DirR PB2
#define Pin_Motor_PwmL PA6
#define Pin_Motor_PwmR PA7

extern HardwareSerial Serial, Serial2;


void MotorsInit();
void Motors(int PwmL, int PwmR);

extern PID PidL, PidR;
extern double PidL_In, PidR_In;
extern double PidL_Sp, PidR_Sp;
void PidTakt(float SpL, float SpR);

void EncodersInit();
void EncodersRead (int &Left, int &Right);

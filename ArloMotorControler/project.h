// project.h
#include "arduino.h"

extern HardwareSerial Serial;

void MotorsInit();
void Motors(int PwmL, int PwmR);

void EncodersInit();
void EncodersRead (int &Left, int &Right);

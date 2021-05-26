//-----------------------------------------------------------------------------
// Project.h - Project-specific includes.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef PROJECT_H
#define PROJECT_H

#include <Servo.h>
extern Servo myservo;

void LppSensorDefaultSetup();

extern int LidarArray_L100;
extern int LidarArray_L80 ;
extern int LidarArray_L60 ;
extern int LidarArray_L40 ;
extern int LidarArray_L20 ;
extern int LidarArray_V   ;
extern int LidarArray_R20 ;
extern int LidarArray_R40 ;
extern int LidarArray_R60 ;
extern int LidarArray_R80 ;
extern int LidarArray_R100;

bool MissionAloys1(TState &S);

#endif

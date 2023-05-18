//-----------------------------------------------------------------------------
// Project.h - Project-specific includes.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef PROJECT_H
#define PROJECT_H

extern HardwareSerial Serial2;
#define CSerial  Serial2       // define Console-serial
#define MyPrintf CSerial.printf

#include "Libs/Commands.h"    // contains code...

#endif

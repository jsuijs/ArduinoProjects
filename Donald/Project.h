//-----------------------------------------------------------------------------
// Project.h - Project-specific includes.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef PROJECT_H
#define PROJECT_H

extern HardwareSerial Serial2;
#define CSerial Serial2       // define Console-serial
#define MyPrintf CSerial.printf

#include "Libs/Commands.h"    // contains code...
#include "Libs/Flags.h"       // contains code...

const int RC_STOP      = 0x3775; // STOP robot - stop button (Function-key -1)

const int RC_F01       = 0x3770; // mark
const int RC_F02       = 0x2745; // audio
const int RC_F03       = 0x2741; // title menu
const int RC_F04       = 0x2749; // sub title
const int RC_F05       = 0x2744; // input
const int RC_F06       = 0x2740; // tv
const int RC_F07       = 0x2746; // hdd
const int RC_F08       = 0x274a; // on line
const int RC_F09       = 0x3769; // zoom
const int RC_F10       = 0x274c; // - volume
const int RC_F11       = 0x3751; // + volume
const int RC_F12       = 0x3750; // store

#endif

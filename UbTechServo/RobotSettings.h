//-----------------------------------------------------------------------------
// RobotSettings.h - project-specific settings
//-----------------------------------------------------------------------------
// Zet in deze file alle instellingen voor je eigen project.
// De 'gewone' includes zet je in Project.h

#ifndef ROBOTSETTINGS_H
#define ROBOTSETTINGS_H

#define ROBOT_JOEP
//#define ROBOT_ALOYS
//#define ROBOT_KAREL

#define CSerial Serial2 // definieer Console-serial.

// Choose i2c for Lidar Preprocessor
#include <Wire.h>
extern  TwoWire Wire2;
#define LppWire Wire2

// IO mapping
#define BUZZER_PIN      PC13

//-----------------------------------------------------------------------------
// TB6612 Motor configuration
const int TB6612_PWML      = PA8;   // Pin 27
const int TB6612_PWMR      = PA11;  // pin 24
const int TB6612_DIRL_A1   = PB15;  // pin 28
const int TB6612_DIRL_A2   = PA10;  // pin 25  jumper pin 23 PA12
const int TB6612_DIRR_B1   = PB14;  // pin 29
const int TB6612_DIRR_B2   = PA9;   // pin 26  jumper pin 31 PB12

//-----------------------------------------------------------------------------
// Timing, odometrie etc.

#define MAIN_TAKT_INTERVAL        20   // miliseconds
#define MAX_SLOPE                 10   // in mm/sec/MAIN_TAKT_INTERVAL

// Encoder parameters (Position.h)
#define ODO_TICK_TO_METRIC       508   // afstand per tick, voor het linkse wiel, 4096 betekent dat 1 tick => 1 mm
#define ODO_TICK_L_R            4096   // correctie voor afwijking wielgrootte rechtse wiel t.o.v. links, 4096 = even groot

#define ODO_HEADING             2592   // vertaal odo ticks naar hoek, groter => meer graden per tick
                                       // ODO_HEADING = (ODO_TICK_TO_METRIC * 917) / WIELBASIS (in mm)
                                       // Als de robot te weinig draait, moet dit getal lager worden

// Motor PID (MPID) parameters
#define MPID_I_MAX      100      // integrator limit
#define MPID_OFFSET     10       // motor offset
#define MPID_SP_GAIN     1.0     // setpoint gain
#define MPID_P_GAIN      1.0
#define MPID_I_GAIN      1
#define MPID_D_GAIN     -1

// Richting P-gain & begrenzing
#define PID_Kp            1000   // hoekerr gain / 4096
#define PID_OUT_CLIP        12   // correctie clip (graden)

#define ROTATE_CLIP_Q8  (10*256) // degrees / tick
#define ROTATE_P_GAIN   (0.08)
#define ROTATE_D_GAIN   (0.05)

// ---------------------------------------------------------
// Instellingen per gebruiker - Activeer bovenaan deze file.
// ---------------------------------------------------------

//=============js===============================
#ifdef ROBOT_JOEP
   //
   // Definitie van functie-toets RC5-codes
   //
   #define RC_STOP         (0x3775)   // STOP robot - stop knop (Functie-toets -1)

   #define RC_F01          (0x3770)   // mark
   #define RC_F02          (0x2745)   // audio
   #define RC_F03          (0x2741)   // title menu
   #define RC_F04          (0x2749)   // sub title
   #define RC_F05          (0x2744)   // input
   #define RC_F06          (0x2740)   // tv
   #define RC_F07          (0x2746)   // hdd
   #define RC_F08          (0x274a)   // on line
   #define RC_F09          (0x3769)   // zoom
   #define RC_F10          (0x274c)   // - volume
   #define RC_F11          (0x3751)   // + volume
   #define RC_F12          (0x3750)   // store
#endif // ROBOT_JOEP

//=============av===============================
#ifdef ROBOT_ALOYS
   //
   // Definitie van functie-toets RC5-codes
   //
   #define RC_STOP         (0x300c)   // 0/1-STOP robot

   #define RC_F01          (0x3001)   // Knop 1
   #define RC_F02          (0x3002)   // Knop 2
   #define RC_F03          (0x3003)   // Knop 3
   #define RC_F04          (0x3004)   // Knop 4
   #define RC_F05          (0x3005)   // Knop 5
   #define RC_F06          (0x3006)   // Knop 6
   #define RC_F07          (0x3007)   // Knop 7
   #define RC_F08          (0x3008)   // Knop 8
   #define RC_F09          (0x3009)   // Knop 9
   #define RC_F10          (0x300a)   // DrKn << >>
   #define RC_F11          (0x300d)   // Mute = knop 13!!
   #define RC_F12          (0x300b)   // Knop CH+P/C
#endif // ROBOT_ALOYS

#endif

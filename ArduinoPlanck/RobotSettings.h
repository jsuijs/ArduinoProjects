//-----------------------------------------------------------------------------
// RobotSettings.h voor Planck
//-----------------------------------------------------------------------------

#ifndef ROBOTSETTINGS_H
#define ROBOTSETTINGS_H

#define CSerial Serial2

// IO mapping
//#define KEYBOARD_PIN    A0
//#define BATTERIJ_PIN    A7
#define BUZZER_PIN      PB5

// Setup i2c for Lidar Preprocessor
#include <Wire.h>
extern  TwoWire Wire2;
#define LppWire Wire2


//-----------------------------------------------------------------------------
// Motor configuration
//#define USE_A4950
//const int  A4950_PWML1 = PA6;
//const int  A4950_PWML2 = PA7;
//const int  A4950_PWMR1 = PB0;
//const int  A4950_PWMR2 = PB1;

#define USE_TB6612
const int TB6612_PWML      = PA8;   // Pin 27
const int TB6612_PWMR      = PA11;  // pin 24
const int TB6612_DIRL_A1   = PB15;  // pin 28
const int TB6612_DIRL_A2   = PA10;  // pin 25  jumper pin 23 PA12
const int TB6612_DIRR_B1   = PB14;  // pin 29
const int TB6612_DIRR_B2   = PA9;   // pin 26  jumper pin 31 PB12

//-----------------------------------------------------------------------------
// Hier de algemene definities
#define MAIN_TAKT_RATE     (1000 / MAIN_TAKT_INTERVAL)   // Hz
#define WIEL_BASIS         ((ODO_TICK_TO_METRIC * 917L) / ODO_HEADING)
#define RAD2GRAD(x) ((float)(x) * 57.2957795)   // uitkomst is float, deze kan evt zonder verlies geschaald worden naar hogere resulotie
#define ACT_SPEED_MM_SEC(ActSpeed) ((ActSpeed * (long)ODO_TICK_TO_METRIC)) / (4 * MAIN_TAKT_INTERVAL);
#define ABS(x) ( (x>=0) ? x : -x )


#define MAIN_TAKT_INTERVAL 20    // miliseconds
//#define MAIN_TAKT_RATE     (1000 / MAIN_TAKT_INTERVAL)
#define MAX_SLOPE           10   // in mm/sec/MAIN_TAKT_INTERVAL

// Encoder parameters (Position.h)
#define ODO_TICK_TO_METRIC       508   // afstand per tick, voor het linkse wiel, 4096 betekent dat 1 tick => 1 mm
#define ODO_TICK_L_R            4096   // correctie voor afwijking wielgrootte rechtse wiel t.o.v. links, 4096 = even groot

#define ODO_HEADING             2700   // vertaal odo ticks naar hoek, groter => meer graden per tick
                                       // ODO_HEADING = (ODO_TICK_TO_METRIC * 917) / WIELBASIS (in mm)
                                       // Als de robot te weinig draait, moet dit getal lager worden

// Motor PID (MPID) parameters (Drive.h)
#define MPID_I_MAX      100      // integrator limit
#define MPID_OFFSET     10       // motor offset
#define MPID_SP_GAIN     1.0 // 3       // setpoint gain
#define MPID_P_GAIN      1.0 //2
#define MPID_I_GAIN      1
#define MPID_D_GAIN     -1

#define PID_Kp            1000   // hoekerr gain / 4096
#define PID_OUT_CLIP        12   // correctie clip (graden)

#define ROTATE_CLIP     (10*256)
#define ROTATE_P_GAIN   (0.08)
#define ROTATE_D_GAIN   (0.05)


#endif

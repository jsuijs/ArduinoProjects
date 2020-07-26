//-----------------------------------------------------------------------------
// RobotSettings.h voor Planck
//-----------------------------------------------------------------------------

// IO mapping
#define KEYBOARD_PIN    A0
#define BATTERIJ_PIN    A7
#define BUZZER_PIN      17

#define SHARP_VOOR_PIN     A4
#define SHARP_RECHTS_PIN   A5

#define ENCODER_L_PIN_B       18 // interrupt 5
#define ENCODER_L_PIN_A       25
#define ENCODER_L_INTERRUPT    5 // pin 18

#define ENCODER_R_PIN_B       19 // interrupt 4
#define ENCODER_R_PIN_A       24
#define ENCODER_R_INTERRUPT    4 // pin 19


// L298 pins:
// Enable pinnen worden vast aan 5V gelegd.
const int  TB6612_PWML = 45;
const int  TB6612_IN1L = 22;
const int  TB6612_IN2L = 26;
const int  TB6612_PWMR = 44;
const int  TB6612_IN1R = 23;
const int  TB6612_IN2R = 27;


// motor mapping
#define Motors(PowerL, PowerR)   Motors_TB6612(PowerL, PowerR);
#define MotorsSetup()            MotorsSetup_TB6612();


#define MAIN_TAKT_INTERVAL 20    // miliseconds
//#define MAIN_TAKT_RATE     (1000 / MAIN_TAKT_INTERVAL)
#define MAX_SLOPE           10   // in mm/sec/MAIN_TAKT_INTERVAL

// Encoder parameters (Position.h)
#define ODO_TICK_TO_METRIC       660   // afstand per tick, voor het linkse wiel, 4096 betekent dat 1 tick => 1 mm
#define ODO_TICK_L_R            4096   // correctie voor afwijking wielgrootte rechtse wiel t.o.v. links, 4096 = even groot

#define ODO_HEADING             2820   // vertaal odo ticks naar hoek, groter => meer graden per tick
                                       // ODO_HEADING = (ODO_TICK_TO_METRIC * 917) / WIELBASIS (in mm)
                                       // Als de robot te weinig draait, moet dit getal lager worden



// Motor PID (MPID) parameters (Drive.h)
#define MPID_I_MAX      100      // integrator limit
#define MPID_OFFSET     10       // motor offset
#define MPID_SP_GAIN     3       // setpoint gain
#define MPID_P_GAIN      2
#define MPID_I_GAIN      1
#define MPID_D_GAIN     -1

#define PID_Kp            1000   // hoekerr gain / 4096
#define PID_OUT_CLIP        12   // correctie clip (graden)

#define ROTATE_CLIP     (10*256)
#define ROTATE_P_GAIN   (0.08)
#define ROTATE_D_GAIN   (0.05)
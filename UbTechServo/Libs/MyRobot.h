//-----------------------------------------------------------------------------
// MyRobot.h - include voor diverse .cpp files.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef MYROBOT_H
#define MYROBOT_H

#include <Arduino.h>
#include <Servo.h>

#include "Libs/Commands.h"    // contains code...

//-----------------------------------------------------------------------------
// Position.cpp
#define TICKS_360_GRADEN   (360L * 256 * 256 / ODO_HEADING)
#define GRAD2RAD(x)        ((float)(x) / 57.2957795)
#define MAIN_TAKT_RATE     (1000 / MAIN_TAKT_INTERVAL)   // Hz
#define WIEL_BASIS         ((ODO_TICK_TO_METRIC * 917L) / ODO_HEADING)
#define RAD2GRAD(x)        ((float)(x) * 57.2957795)   // uitkomst is float, deze kan evt zonder verlies geschaald worden naar hogere resulotie
#define ACT_SPEED_MM_SEC(ActSpeed) ((ActSpeed * (long)ODO_TICK_TO_METRIC)) / (4 * MAIN_TAKT_INTERVAL);
#define ABS(x)             ( (x>=0) ? x : -x )
#define NORM_Q8            (360L * 256)

#define FRAME_END             0xC0    /* indicates end of packet */
#define FRAME_START           0xC1    /* indicates start of packet */

template <typename T> inline
T ABSOLUTE(const T& v) { return v < 0 ? -v : v; }

//-----------------------------------------------------------------------------
class TPosition
{
   public:
      TPosition();
      void init() { Reset(); }
      void Takt();
      void Reset();

      void OdoGet(int &OdoL_out, int &OdoR_out, int &OdoT_out) ;
      void Print();

      int  ActSpeedL, ActSpeedR;    // in odo_ticks per MAIN_TAKT_INTERVAL
      int  XPos;  // in mm
      int  YPos;
      int  Hoek;  // in graden

      long HoekHires() { return VarRobotDegrees_q8; }

   private:
      // de robot positie.
      long int VarRobotXPos_q10;    // in 1/1024 mm (ca 1 um)
      long int VarRobotYPos_q10;    // in 1/1024 mm (ca 1 um)
      long int VarRobotDegrees_q8;  // in 360*256 /circel (graden*256)

      long int OdoL_ticks;          // afstand in odo_ticks  - LET OP - alleen voor hoek; var maakt sprong van '360 graden' ticks.
      long int OdoR_ticks;          // afstand in odo_ticks  - LET OP - alleen voor hoek;

      long int OdoL_q10;            // afstand in mm * 1024 (ongeveer um)
      long int OdoR_q10;            // afstand in mm * 1024 (ongeveer um)
      long int OdoT_q10;            // afstand in mm * 1024 (ongeveer um) (gemiddelde van L+R, absolute waarde!)

      // private methods
      void Update();
};
extern TPosition Position;

//-----------------------------------------------------------------------------
class TDrive
{
   enum TDiveMode { UNDEFINED, M_PWM, M_SPEED_LR, M_SPEED_ROTATION, M_SPEED_HEADING, M_XY, M_ROTATE, M_ARC, M_STOP };

   public:
      TDrive();
      void init();
      void Takt();
      bool IsDone();

      // bewegingen
      void Pwm(int PwmL, int PwmR);
      void SpeedLR(int SpeedL, int SpeedR);
      void SpeedRotation(int Speed, int Rotation_q8);
      void SpeedHeading(int Speed, int Heading);
      void XY(int X, int Y, int Speed, int EndSpeed);
      void RotateHeading(int Heading, int RotateClip = ROTATE_CLIP_Q8);
      void Rotate(int Degrees, int RotateClip = ROTATE_CLIP_Q8);
      void ArcHeading(int Heading, int Radius, int Speed, int EndSpeed);
      void Arc(int Degrees, int Radius, int Speed, int EndSpeed);
      void Stop();

   private:
      TDiveMode DriveMode;    // actief type aansturing
      int Param1;             // Paramers van actieve aansturing type
      int Param2;
      int Param3;
      int Param4;

      bool IsDoneFlag;        // Movement is gereed
      bool NewMovement;

      int SollSpeedL, SollSpeedR; // Snelheid (in mm/sec) die we nastreven, verandering begrensd door MaxSlope

      int MaxSlope;

      void SpeedLRTakt(bool FirstCall, int SpeedL, int SpeedR, int MaxSlopeP);
      bool SpeedRotationTakt(bool FirstCall, int InSpeed, int InRotation_q8);
      bool SpeedHeadingTakt(bool FirstCall, int InSpeed, int InHeading);
      bool XYTakt(bool FirstCall, int TargetX, int TargetY, int Speed, int EndSpeed);
      bool RotateRelTakt(bool FirstCall, int DeltaDegrees, int RotateClip_q8);
      bool ArcRelTakt(bool FirstCall, int DeltaDegrees, int Radius, int Speed, int EndSpeed);
      bool StopTakt(bool FirstCall);
};
extern TDrive Driver;

//-----------------------------------------------------------------------------
class TState
{
   public:
      TState();
      void Reset();

      void Update(const char *InName, bool Verbose=true);

      int StateTime();

      int  State;
      bool NewState;

      int  Param1;   // user param

   private:
      int PrevState;
      int StateStartTime;
};

//-----------------------------------------------------------------------------
class TBuzzer
{
   public:
      TBuzzer(int Pin);
      void Takt();

      void Beep(int duration, int number_of_beeps = 1);
      void BeepWait(int duration, int number_of_beeps = 1);
      void Wait();

   private :
      int BuzzerPin;
      volatile int BeepState, BeepDuration;
      int BeepCountDown, Toggle;
};
extern TBuzzer Buzzer;

//-----------------------------------------------------------------------------
class TFlags
{
   public:
      TFlags(int NrFlags); // NrFlags 32 means 0..31

      bool IsSet(int Nr);
      void Set(int Nr, bool Value);

      void Dump();

   private:
      int *FlagWords;
      int NrFlagWords;  // WordIx + 1
      int WordIx, BitIx;

      bool SetIx(int Nr);
};
extern TFlags Flags;

bool ServoSlope(Servo &S, int Setpoint, int Step);

//-----------------------------------------------------------------------------
// Encoders
void InitStmEncoders();
void ReadStmEncodersDelta(int &Left, int &Right);

//-----------------------------------------------------------------------------
// Motors.cpp
void SetupMotors();
void Motors(int PwmL, int PwmR);

//-----------------------------------------------------------------------------
// MotorController.cpp
void MotorController(int SetpointL, int SetpointR);

// Utilities.cpp
int EenparigVertragen( int Afstand, int SetSpeed, int EndSpeed, int Vertraging);
long Clip(long input, long min, long max);
void Slope(int &SlopeInOut, int Setpoint, int Step);
long NormHoek(long hoek, long Norm);
void Cartesian2Polar(long &hoek, int &afstand, int x, int y);
void I2cClearBus(int PinSda, int PinScl);

// ProgrammaTakt.cpp
void ProgrammaTakt();

// RcDispatch.cpp
void RcDispatch(int &RcData);
int  PfKeyGet();
void PfKeySet(int InKey);

#endif

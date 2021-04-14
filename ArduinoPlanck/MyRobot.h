// file: MyRobot.h - include voor diverse .cpp files.
#ifndef MYROBOT_H
#define MYROBOT_H

//-----------------------------------------------------------------------------
// Position.cpp
//-----------------------------------------------------------------------------
#include "Arduino.h"
#include "RobotSettings.h"
#include "Libs/LppMaster.h"

#define TICKS_360_GRADEN (360L * 256 * 256 / ODO_HEADING)
#define GRAD2RAD(x) ((float)(x) / 57.2957795)

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

      long HoekHires();

   private:

      // private vars

      // de robot positie.
      long int VarRobotXPos;    // in 1/1024 mm (ca 1 um)
      long int VarRobotYPos;    // in 1/1024 mm (ca 1 um)
      long int VarRobotHoek;    // in 360*256 /circel (graden*256)

      long int OdoL_ticks;      // afstand in odo_ticks  - LET OP - alleen voor hoek; var maakt sprong van '360 graden' ticks.
      long int OdoR_ticks;      // afstand in odo_ticks  - LET OP - alleen voor hoek;

      long int OdoL;            // afstand in mm * 1024 (ongeveer um)
      long int OdoR;            // afstand in mm * 1024 (ongeveer um)
      long int OdoT;            // afstand in mm * 1024 (ongeveer um) (gemiddelde van L+R, absolute waarde!)

      // private methods
      void Update();
};

extern TPosition Position;

//-----------------------------------------------------------------------------
// Drive.cpp
//-----------------------------------------------------------------------------

// Constante per bewegingstype (DriveMode) die we ondersteunen.
enum TDiveMode { UNDEFINED, M_PWM, M_SPEED_LR, M_SPEED_HEADING, M_XY, M_ROTATE, M_ARC, M_STOP };

class TDrive
{
   public:

      TDrive();
      void init();
      void Takt();
      bool IsDone();

      // bewegingen
      void Pwm(int PwmL, int PwmR);
      void SpeedLR(int SpeedL, int SpeedR);
      void SpeedHeading(int Speed, int Heading);
      void XY(int X, int Y, int Speed, int EndSpeed);
      void Rotate(int Heading);
      void Arc(int Heading, int Radius, int Speed, int EndSpeed);
      void Stop();


      int SollSpeedL, SollSpeedR; // Snelheid (in mm/sec) die we nastreven, verandering begrensd door MaxSlope

   private:

      // private vars
      TDiveMode DriveMode;    // actief type aansturing
      int Param1;             // Paramers van actieve aansturing type
      int Param2;
      int Param3;
      int Param4;

      bool IsDoneFlag;        // Movement is gereed
      bool NewMovement;

      int MaxSlope;


      // private methods
      void UpdateSpeedSP(int InSpeedL, int InSpeedR, int MaxSlopeP);

      void SpeedLRTakt(bool FirstCall, int SpeedL, int SpeedR, int MaxSlopeP);
      bool SpeedHeadingTakt(bool FirstCall, int InSpeed, int InHeading);
      bool XYTakt(bool FirstCall, int TargetX, int TargetY, int Speed, int EndSpeed);
      bool RotateTakt(bool FirstCall, int InHeading);
      bool ArcTakt(bool FirstCall, int Heading, int Radius, int Speed, int EndSpeed);
      bool StopTakt(bool FirstCall);

};

extern TDrive Driver;


//-----------------------------------------------------------------------------
class TState
{
   public:

      TState() { Reset(); }

      void Update(const char *InName) {
         if (PrevState != State) {
            CSerial.printf("%s state %d -> %d\n", InName, PrevState, State);

            PrevState      = State;
            NewState       = true;
            StateStartTime = millis();
         }
      }

      void Reset() {
         State  = 0;
         PrevState = -1;
      }

      int StateTime() {
         return millis() - StateStartTime;
      }

      int  State;
      bool NewState;

      int  Param1;   // user param

   private:
      int PrevState;
      int StateStartTime;
};


// Encoders
//void EncoderSetup();
//void EncoderRead (int &LeftDelta, int &RightDelta);
//void EncoderPrint();
//void IsrEncoderL();
//void IsrEncoderR();

void InitStmEncoders();
void ReadStmEncodersDelta(int &Left, int &Right);

extern volatile int EncoderLTeller, EncoderRTeller;  // aantal flanken

// Motors.cpp
void SetupMotors();
void Motors(int PwmL, int PwmR);

// Motors.cpp
//void MotorsSetup_A4950();
//void Motors_A4950(int PwmL, int PwmR);

// MotorController.
void MotorController(int SetpointL, int SetpointR);

// Utilities.cpp
int EenparigVertragen( int Afstand, int SetSpeed, int EndSpeed, int Vertraging);
long Clip(long input, long min, long max);
void Slope(int &SlopeInOut, int Setpoint, int Step);
long NormHoek(long hoek, long Norm);
void Cartesian2Polar(long &hoek, int &afstand, int x, int y);

// main ino
extern bool SecondLoggingOn;

// ProgrammaTakt.cpp
void ProgrammaTakt();

// RcDispatch.cpp
void RcDispatch(int &RcData);
int  PfKeyGet();
void PfKeySet(int InKey);

#endif

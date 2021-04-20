// file: MyRobot.h - include voor diverse .cpp files.
#ifndef MYROBOT_H
#define MYROBOT_H

//-----------------------------------------------------------------------------
// Position.cpp
//-----------------------------------------------------------------------------
#include "Arduino.h"

#include "Libs/LppMaster.h"   // contains code...
#include "Libs/Commands.h"    // contains code...

#define TICKS_360_GRADEN   (360L * 256 * 256 / ODO_HEADING)
#define GRAD2RAD(x)        ((float)(x) / 57.2957795)
#define MAIN_TAKT_RATE     (1000 / MAIN_TAKT_INTERVAL)   // Hz
#define WIEL_BASIS         ((ODO_TICK_TO_METRIC * 917L) / ODO_HEADING)
#define RAD2GRAD(x)        ((float)(x) * 57.2957795)   // uitkomst is float, deze kan evt zonder verlies geschaald worden naar hogere resulotie
#define ACT_SPEED_MM_SEC(ActSpeed) ((ActSpeed * (long)ODO_TICK_TO_METRIC)) / (4 * MAIN_TAKT_INTERVAL);
#define ABS(x)             ( (x>=0) ? x : -x )

template <typename T> inline
T ABSOLUTE(const T& v) { return v < 0 ? -v : v; }

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

      long HoekHires() { return VarRobotHoek; }

   private:

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
      void RotateRel(int Degrees);
      void Arc(int Heading, int Radius, int Speed, int EndSpeed);
      void Stop();

      int SollSpeedL, SollSpeedR; // Snelheid (in mm/sec) die we nastreven, verandering begrensd door MaxSlope

   private:

      TDiveMode DriveMode;    // actief type aansturing
      int Param1;             // Paramers van actieve aansturing type
      int Param2;
      int Param3;
      int Param4;

      bool IsDoneFlag;        // Movement is gereed
      bool NewMovement;

      int MaxSlope;

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

      void Update(const char *InName, bool Verbose=true) {
         NewState = false;
         if (PrevState != State) {
            if (Verbose) CSerial.printf("%s state %d -> %d\n", InName, PrevState, State);

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

//-----------------------------------------------------------------------------
class TFlags
{
   public:

      TFlags(int NrFlags) {
         SetIx(NrFlags-1); // 32 means 0..31
         NrFlagWords = WordIx + 1;
         FlagWords   = (int *)malloc(NrFlagWords * sizeof(int));
         for (int i=0; i<NrFlagWords; i++) FlagWords[i] = 0;;
      }

      bool IsSet(int Nr) {
         if (!SetIx(Nr)) return false;
         return ((FlagWords[WordIx] & (1<<BitIx)) != 0);
      }

      void Set(int Nr, bool Value) {
         if (!SetIx(Nr)) {
            CSerial.printf("ERROR setting flag %d\n", Nr);
            return;
         }
         if (Value) {
            FlagWords[WordIx] |= (1<<BitIx);
         } else {
            FlagWords[WordIx] &= 0xFFFFFFFF ^ (1<<BitIx);
         }
         CSerial.printf("Flag %d set to %d (%d %d %d)\n", Nr, Value, WordIx, BitIx, NrFlagWords);
      }

      void Dump() {
         CSerial.printf("NrFlagWords: %d\n", NrFlagWords);
         for (int i=0; i<NrFlagWords; i++) CSerial.printf("%08x ", FlagWords[i]);
         CSerial.printf("\n");
      }

   private:
      int *FlagWords;
      int NrFlagWords;  // WordIx + 1
      int WordIx, BitIx;

      bool SetIx(int Nr) {  // return true if Nr is valid
         WordIx = Nr / 32;
         BitIx  = Nr - 32 * WordIx;
         if (WordIx >= NrFlagWords) return false;  // out of range
         if (!FlagWords)            return false;  // no vars malloc'd
         return true;
      }
};

extern TFlags Flags;

//-----------------------------------------------------------------------------
// Encoders
void InitStmEncoders();
void ReadStmEncodersDelta(int &Left, int &Right);

extern volatile int EncoderLTeller, EncoderRTeller;  // aantal flanken

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

// ProgrammaTakt.cpp
void ProgrammaTakt();

// RcDispatch.cpp
void RcDispatch(int &RcData);
int  PfKeyGet();
void PfKeySet(int InKey);

#endif

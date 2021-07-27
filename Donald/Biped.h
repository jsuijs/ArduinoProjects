//-----------------------------------------------------------------------------
// Biped.h - Biped ActionEngine
//-----------------------------------------------------------------------------
// The ActionEngine is based on sequences of actions.
//
// * Most actions set 4 servo's based on the J* fields of the action and the
//   5th servo  (direction) based on the command (Cmd).
// * The Sequence parameter of Takt indicates which Sequence is requisted.
// * In normal operation, an action is executed and after the WaitTime (WTime),
//   it advances to the 'Next' Action.
// * When another Sequence is requested, the current sequence is follows the
//   'NextStop' sequence until the IDLE_IX state (0) and then the new Sequence
//   is started.
//-----------------------------------------------------------------------------

const char CMD_IDLE           = 0;
const char CMD_4SERVO_STEP    = 1;
const char CMD_4S_TURN        = 2;
const char CMD_4S_ANTI_TURN   = 3;

struct TAction {
  short  Cmd;              // Command (what action)
  short  Next, NextStop;   // Next (sequence) for normal and STOP operation
  short  J0, J1, J2, J3;   // Degrees for Joints
  short  StepTime;         // servo turn-time in 10ms steps
  short  WaitTime;         // wait-time (normally >= StepTime) in 10ms steps
} ;

#define TN2 16 // turn stop (halfway)

TAction Actions[] = {
   // ------------------------- Next NextStop J0    J1    J2    J3    STime WTime
#define IDLE_IX 0
   /*   0 */ { CMD_IDLE,           0,    0,    0,    0,    0,    0,     0,   10},  // IDLE

#define GP0 1
   /*   1 */ { CMD_4SERVO_STEP,    0,    0,   13,  -11,   -6,    9,    50,  100},  // GenPosture_0 - Voeten bij elkaar, rust

#define GP1 2  // start of large step
   /*   2 */ { CMD_4SERVO_STEP,    3,  GP0,    12,    1,  -13,   17,   50,  100},  // GenPosture_1 - voeten bij elkaar, voorvoet vrij
   /*   3 */ { CMD_4SERVO_STEP,    4,    4,     7,   -4,   45,  -30,   50,  100},  // GenPosture_2 - voorvoet max reach, vlak op grond
   /*   4 */ { CMD_4SERVO_STEP,    5,    5,   -18,   18,   16,  -26,   60,  120},  // GenPosture_3 - gewicht naar midden
   /*   5 */ { CMD_4SERVO_STEP,    6,    6,   -40,   50,  -19,   10,   60,  120},  // GenPosture_4 - gewicht naar voor
   /*   6 */ { CMD_4SERVO_STEP,    7,    7,   -15,   50,  -22,   10,   50,  100},  // P5 - achtervoet strekken (omklappen)
   /*   7 */ { CMD_4SERVO_STEP,    8,    8,    11,   50,  -22,   39,   50,  100},  // P6 - kantel gewicht naar voren
   /*   8 */ { CMD_4SERVO_STEP,    9,    9,    11,   50,  -22,   46,   50,  100},  // P7 - achtervoet vrij
   /*   9 */ { CMD_4SERVO_STEP,   10,   10,   -32,   48,  -23,   50,   50,  100},  // Posture_6 - achtervoet vrij (omklappen)
   /*  10 */ { CMD_4SERVO_STEP,  GP1,  GP1,   -16,   12,  -30,   34,   50,  100},  // Posture_7 - achtervoet bijtrekken

#define TN1 11 // start of turn
   /*  11 */ { CMD_4SERVO_STEP,   12,   12,    14,    1,  -10,  13,    50,  100}, // T1 - Turn voor vrij, 0 graden
   /*  12 */ { CMD_4S_TURN,       13,   13,    14,    1,  -10,  13,    50,  100}, // T1_L20 - Turn voor vrij, links 20 graden
   /*  13 */ { CMD_4S_TURN,       14,  TN2,   -22,   19,  -28,  40,    50,  100}, // T2_L20 - Turn achter vrij, links 20 graden
   /*  14 */ { CMD_4S_ANTI_TURN,  15,   15,   -22,   19,  -28,  40,    50,  100}, // T2_R20 - Turn achter vrij, rechts 20 graden
   /*  15 */ { CMD_4S_ANTI_TURN, TN1,  GP0,    14,    1,  -10,  13,    50,  100}, // T1_R20 - Turn voor vrij, rechts 20 graden

//==>> #define TN2 16 // turn stop (halfway)
   /*  16 */ { CMD_4SERVO_STEP,  TN1,  TN1,   -22,   19,  -28,  40,    50,  100}, // T2 - Turn achter vrij, 0 graden

//==>> dummy (no comma)
   /* 999 */ { 99, 99, 99, 99, 99, 99, 99, 99, 99}  // Dummy
   };

class TActionEngine
{
   public :
      TActionEngine() : Action(Actions[0])
         {
            ActionIx    = 0;
            SpeedFactor = 10; // StepTime / Waittime factor (default 10 ms)
         }

      void SetWait(int Delay_ms) { WaitEnd = millis() + Delay_ms;          }
      bool WaitDone()            { return (((int)millis()) - WaitEnd) > 0; }
      void TestPose(int Ix)      { ExecuteAction(Actions[Ix]);             }

      void Takt(int NewSequence);

      int SpeedFactor;

   private :
      void NewAction(int NewIx);
      void ExecuteAction(TAction &Action);
      bool Wait();

      int ActionIx;
      int WaitEnd;
      TAction &Action;
      char WalkHeading, TurnHeading;
      int ActiveSequence;
      bool StopFlag;

} ActionEngine;

//-----------------------------------------------------------------------------
// TActionEngine::NewAction - go to new action & execute it
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TActionEngine::NewAction(int NewIx)
   {
      if (ActionIx == NewIx) return;   // no action required.

      // Step to requested action
      CSerial.printf("NewAction %d -> %d\n", ActionIx, NewIx);
      ActionIx = NewIx;
      Action   = Actions[ActionIx];

      // Execute action
      ExecuteAction(Action);
      SetWait(Action.WaitTime * SpeedFactor);
   }

//-----------------------------------------------------------------------------
// TActionEngine::ExecuteAction - Execute current action
//-----------------------------------------------------------------------------
// Note: Separate function with InAction as param for TestPose
//-----------------------------------------------------------------------------
void TActionEngine::ExecuteAction(TAction &InAction)
   {
      int STime = InAction.StepTime * SpeedFactor;

      if (ActionIx == 0) {
         // No commands to servo's
      } else {
         // Translate degrees to raw (with offset) and map from Joint # to servo #
         UbtServo.setServoAngle(3,  1.11 * InAction.J0 - 10, STime);
         UbtServo.setServoAngle(4, -1.11 * InAction.J1 -  4, STime);
         UbtServo.setServoAngle(2,  1.11 * InAction.J2 -  8, STime);
         UbtServo.setServoAngle(1, -1.11 * InAction.J3 -  6, STime);
      }

      // Direction-servo
      switch (InAction.Cmd) {
         case CMD_IDLE           : /* No commands to servo's */                           break;
         case CMD_4S_TURN        : UbtServo.setServoAngle(5,  1.11 * TurnHeading, STime);  break;
         case CMD_4S_ANTI_TURN   : UbtServo.setServoAngle(5, -1.11 * TurnHeading, STime);  break;
         default                 : UbtServo.setServoAngle(5,  1.11 * WalkHeading, STime);  break;
      }
   }

//-----------------------------------------------------------------------------
// TActionEngine::Takt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TActionEngine::Takt(int NewSequence)
   {
      if (WaitDone() == false) return;  // Wait-time of previous action

      // Next action
      CSerial.printf("Takt NewSequence: %d, ActiveSequence: %d, ActionIx: %d\n", NewSequence, ActiveSequence, ActionIx);

      if (ActionIx == IDLE_IX) {
         // we're idle => start new sequence
         ActiveSequence = NewSequence;
         switch(ActiveSequence) {
            case 0 : NewAction(IDLE_IX);  SetWait(100);        break;   // remain idle
            case 1 : NewAction(GP0);                           break;   // default pose & then idle
            case 2 : NewAction(GP1);                           break;   // Large steps
            case 3 : NewAction(TN1);      TurnHeading =  10;   break;   // Turn left
            case 4 : NewAction(TN1);      TurnHeading = -10;   break;   // Turn right
         }
      } else {
         // we're NOT idle, so next step
         if (ActiveSequence == NewSequence) {
            // Continue this sequence
            NewAction(Action.Next);
         } else {
            // Follow stop-sequence
            NewAction(Action.NextStop);
         }
      }
   }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Basic BIPED interface (for serial control)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const unsigned char FRAME_END   = 0xC0;    /* indicates end of packet */
const unsigned char FRAME_START = 0xC1;    /* indicates start of packet */

//-----------------------------------------------------------------------------
// BipedSet - set 5 servos
//-----------------------------------------------------------------------------
// 6 Params time, s1 ... s5
//-----------------------------------------------------------------------------
void BipedSet(int Param[])
{
   int Time = Param[0];

   for (int Servo=1; Servo<6; Servo++) {
      UbtServo.setServoAngle(Servo, Param[Servo], Time);
   }
}

//-----------------------------------------------------------------------------
// BipedRead - read 5 servos
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BipedRead()
{
   CSerial.printf("%cREAD", FRAME_START);
   for (int Servo=1; Servo<6; Servo++) {
      CSerial.printf(" %d", UbtServo.readServoAnglePD(Servo));
   }
   CSerial.printf("%c\n", FRAME_END);
}

// Biped-h - specific commands to interact with the biped robot
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Biped ActionEngine
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

const char CMD_IDLE           = 0;
const char CMD_4SERVO_STEP    = 1;
const char CMD_4S_TURN        = 2;
const char CMD_4S_ANTI_TURN   = 3;

const short EVENT_IDLE         = 0;    // Robot in default position
const short EVENT_STEP         = 1;    // Step to NextState
const short EVENT_TURN_STOP1   = 2;    // Turn-done check 1 (goto T2_0 if true)
const short EVENT_TURN_STOP2   = 3;    // Turn-done check 2 (goto T1_0 if true)


struct TAction {
  char  Cmd;      // Command (what action)
  char  Event;    // Event when action is done (NextState or Special)
  char  Next;     // Next (no stop)
  char  NextStop; // Next when STOP is set
  char  J0;       // Degrees for Joint 0 (which maps to servo3)
  char  J1;
  char  J2;
  char  J3;
  char  StepTime; // servo turn-time in 10ms steps
  char  WaitTime; // wait-time (normally >= StepTime) in 10ms steps
} ;


TAction Actions[] = {
   //                               Next  NextStop J0    J1    J2    J3    STime WTime
   { CMD_IDLE,          EVENT_IDLE,    0,    0,    0,    0,    0,    0,     0,   10},   // IDLE
   { CMD_4SERVO_STEP,   EVENT_STEP,    0,    0,    3,    4,    5,    1,    50,   70},   // GP0 - description
   };

class TActionEngine
{
   public :
      TActionEngine() : Action(Actions[0])
         {
            ActionIx = 0;
         }

      void SetWait(int Delay_ms) { WaitEnd = millis() + Delay_ms;          }
      bool WaitDone()            { return (((int)millis()) - WaitEnd) > 0; }

      void Takt(int Mode);

   private :
      void ExecuteAction();
      bool Wait();
      void Transition(int Mode);

      int ActionIx;
      int WaitEnd;
      TAction &Action;
      char WalkHeading, TurnHeading;
      int ActiveMode;
      bool StopFlag;

} ActionEngine;

//-----------------------------------------------------------------------------
// TActionEngine::ExecuteAction - Execute current action
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TActionEngine::ExecuteAction()
   {
      if (Action.Cmd == CMD_IDLE) {
         // No commands to servo's
      } else {
         // Translate degrees to raw (with offset) and map to servo's
         UbtServo.setServoAngle(3,  1.11 * (Action.J0 - 10), Action.StepTime);
         UbtServo.setServoAngle(4, -1.11 * (Action.J1 -  4), Action.StepTime);
         UbtServo.setServoAngle(2,  1.11 * (Action.J2 -  8), Action.StepTime);
         UbtServo.setServoAngle(1, -1.11 * (Action.J3 -  6), Action.StepTime);
      }

      switch (Action.Cmd) {
         case CMD_IDLE           : /* No commands to servo's */                                     break;
         case CMD_4S_TURN        : UbtServo.setServoAngle(5,  1.11 * TurnHeading, Action.StepTime); break;
         case CMD_4S_ANTI_TURN   : UbtServo.setServoAngle(5, -1.11 * TurnHeading, Action.StepTime); break;
         default                 : UbtServo.setServoAngle(5,  1.11 * WalkHeading, Action.StepTime); break;
      }
   }

//-----------------------------------------------------------------------------
// TActionEngine::Takt -
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TActionEngine::Takt(int NewMode)
   {
      if (WaitDone() == false) return;  // still waiting

      Transition(NewMode);  // go to next action

      ExecuteAction();  // Execute action & set wait-time
   }

//-----------------------------------------------------------------------------
// TActionEngine::Transition - Move to next state
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TActionEngine::Transition(int NewMode)
   {
      int NewIx = -1;

      bool StopFlag = (ActiveMode != NewMode);

      switch(Action.Event) {

         case EVENT_STEP       : { // Step to NextState
            NewIx = Action.Next;
         }
         break;

         case EVENT_IDLE       : {  // Robot in default position => transition to other modes
            ActiveMode = NewMode;
            switch(ActiveMode) { // ActiveMode is Ix of first sequence step
               case 0 : {  // Remain idle.
               }
               break;
            }
         }
         break;

         case EVENT_TURN_STOP1 : { // Turn-done check 1 (goto T2_0 if true)
         }
         break;

         case EVENT_TURN_STOP2 : { // Turn-done check 2 (goto T1_0 if true)
         }
         break;

         default : {
            CSerial.printf("Unknown event %d (ActionIx %d)\n", Action.Event, ActionIx);
            NewIx = 0;
         }
         break;
      }

      if (NewIx < 0) {
         CSerial.printf("Invalid NewIx %d at event %d (ActionIx %d)\n", NewIx, Action.Event, ActionIx);
         NewIx = 0;
      }

      CSerial.printf("ExecutionTakt Event %d, ActionIx %d -> %d", Action.Event, ActionIx, NewIx);
   }


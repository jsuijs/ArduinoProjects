//-----------------------------------------------------------------------------
class TState
{
   public:

      //-----------------------------------------------------------------------
      // TState - Constructor
      //-----------------------------------------------------------------------
      TState() {
         Reset();
      }

      //-----------------------------------------------------------------------
      // Reset - Back to state 0
      //-----------------------------------------------------------------------
      void Reset() {
         State  = 0;
         PrevState = -1;
      }

      //-----------------------------------------------------------------------
      // Update - housekeeping, call this at the beginning of each takt.
      //-----------------------------------------------------------------------
      void Update(const char *InName, bool Verbose=true) {
         // * detect changes in State (from previous call)
         // * report state changes when Verbose is true
         // * update NewState
         NewState = false;
         if (PrevState != State) {
            if (Verbose) MyPrintf("%s state %d -> %d\n", InName, PrevState, State);

            PrevState      = State;
            NewState       = true;
            StateStartTime = millis();
         }
      }

      //-----------------------------------------------------------------------
      // InvalidState - Report invalid state
      //-----------------------------------------------------------------------
      bool InvalidState(const char *FName) {
         // Returns true, so the error can be handled with a single line statement:
         //
         //    default : return S.InvalidState(__FUNCTION__);   // Report invalid state & end mission
         //
         MyPrintf("Error: invalied state in %s (%d)\n", FName, State);
         return true;
      }

      //-----------------------------------------------------------------------
      // StateTime - return # of ms since we entered this state
      //-----------------------------------------------------------------------
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




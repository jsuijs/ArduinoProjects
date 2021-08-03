//-----------------------------------------------------------------------------
// ProgramTakt.cpp
//-----------------------------------------------------------------------------
#include "Project.h"

// prototypes
extern int InSequence;

//-----------------------------------------------------------------------------
// ProgramTakt - program-selection & call the program (mission)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ProgramTakt()
{  static TState Program;

   int ch = PfKeyGet();
   if (ch) {
      // button-press

      MyPrintf("Key: %d\n", ch);
      if (ch == -1) {
         Program.Reset();           // reset, stop running program / programma 'stand-still'.
      } else {
         if (Program.State == 0) {  // other pfkeys only work when we're idle
            Program.State = ch;
         }
      }
   }

   Program.Update("Program", Flags.IsSet(10));
   if (Program.NewState) {
//      LppSensorDefaultSetup();      // re-load Lidar default configuration
//      MissonS.Reset();              // reset mission statemachine
   }

   // Call active program 1 trough 12
   switch(Program.State) {

      case 0 : { // Program: stand-still
         if (Program.NewState) {
            //Driver.Pwm(0, 0); // only on entry, so CLI-commands can be used in this state.
            //Lpp.Stop();
            InSequence = 0;
         }
      }
      break;

      case 1 : { // Program:
         //if (Rijden1Takt(Program.NewState)) Program.State = 0;
      }
      break;



      default : {
         MyPrintf("ProgramTakt: invalid program %d\n", Program);
         Program.Reset();
      }
      break;
   } // einde van switch
}

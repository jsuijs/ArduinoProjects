// StateMachine.ino -

//-----------------------------------------------------------------------------
// DriveTakt - State machine voor aansturing motor.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DriveTakt()
{
   static int State, PrevState = -1;
   bool NewState = false;
   int EncL, EncR;

   static unsigned long ts;

  // Report state changes
  if (PrevState != State)
  {
    printf("DriveTakt State: %d -> %d\n", PrevState, State);
    PrevState = State;
    NewState = true;
  }

   // state machine
   switch (State)
   {
      case 0 : {  // State: wait for button
         if (NewState) {
            Robot.Motors(0, 0);
            Robot.RgbLeds(0);
            printf("Druk op knop\n");
         }

         if (Robot.CalcKey()) {
            State = 10;
         }
      }
      break;

      case 10 : {  // State: start delay
         if (NewState) {
            ts = millis();
            Robot.RgbLeds(1);
         }

         if ((millis() - ts) > 1000) State = 20;
      }
      break;

      case 20 : {  // State: forth
         if (NewState) {
            Robot.EncodersReset();
            Robot.Motors(200, 200);
            Robot.RgbLeds(2);
         }

         Robot.EncodersRead(EncL, EncR);
         if ((EncL + EncL) > 13333) State = 30;
      }
      break;

      case 30 : {  // State: back
         if (NewState) {
            Robot.Motors(-200, -200);
            Robot.RgbLeds(4);
         }

         Robot.EncodersRead(EncL, EncR);
         if ((EncL + EncL) < 0) State = 0;
      }
      break;


      default : {
         printf("DriveTakt: invalid state %d\n", State);
         State = 0;
         break;
      }
   } // switch end
}

// StateMachine.ino - 

//-----------------------------------------------------------------------------
// MotorTakt - State machine voor aansturing motor.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MotorTakt()
{ 
  static int State, PrevState = -1;

  // rapporteer status bij state overgang
  if (PrevState != State) {  
    PrevState = State;  
    printf("MotorTakt State: %d\n", State);
  }

  // state machine
  switch(State) {
  case 0 : // State: rust
    {  
      // State actie

      // Condities
      if (1) { // Knop ingedrukt
      }
      break;
    }

  default : 
    {
      printf("MotorTakt: ongeldige state %d\n", State);
      State = 0;
      break;
    }
  } // einde van switch 
}




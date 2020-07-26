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
      Motors(0,0);
      
      // Condities
      if (digitalRead(KNOP) == 0) { // Knop ingedrukt
        State = 1;  // rijd tot object
      }
      break;
    }  
    case 1 : // State: Rijden tot object
    {  
      // State actie
      Motors(200,200);
      
      // Condities
      if (Centimeters < 30) { // Object 
        State = 0;  // stop
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




// StateMachine.ino - 

//-----------------------------------------------------------------------------
// RijdenTakt - State machine voor aansturing motor.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RijdenTakt()
{ 
  static int State, PrevState = -1;
  bool NewState = false;
  static int Richting;  // gewenste rijrichting
  static int Doel;      // Encoder L + R op doel-locatie
  static int Tijd;      

  // rapporteer status bij state overgang
  if (PrevState != State) {  
    PrevState = State;  
    printf("RijdenTakt State: %d\n", State);
    NewState = true;
  }

  // state machine
  switch(State) {
  case 0 : // State: rust
    {  
      // State actie
      Motors(0,0);

      // Condities
      if (digitalRead(KNOP) == 0) { // Knop ingedrukt
        State = 5;  // rijd heen
      }
      break;
    }  

  case 5 :       // State actie : rij heen
    {
      if (NewState) {
        // Eerste aanroep van state
        Tijd = 300;  // * 10ms (MainTakt)
      }
      
      // vaste PWM voor motors
      Motors(115, 115);

      // Conditie
      Tijd --;
      if (Tijd <= 0) {  // lang genoeg gereden?
        State = 0;  // ja => klaar
      }
      break;
    }

  default : 
    {
      printf("RijdenTakt: ongeldige state %d\n", State);
      State = 0;
      break;
    }
  } // einde van switch 
}





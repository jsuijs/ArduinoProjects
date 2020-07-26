// StateMachine.ino - 

//-----------------------------------------------------------------------------
// RijdenTakt - State machine voor aansturing motor.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RijdenTakt()
{ 
  static int State, PrevState = -1;
  bool NewState = false;
  
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
        State = 1;  // rijd tot object
      }
      break;
    }  

  case 1 :       // State actie : volg wand
    {
      int Afwijking = Centimeters - 30;   // correctie tov gewenste afstand
      int Correctie = Afwijking * 7;      // versterking
      Motors(150 + Correctie, 150 - Correctie);

      // Conditie
      if (Afwijking < -15) { // Te dicht bij de wand?
        State = 91;      
      }
      // Conditie
      if (Afwijking > 15) { // Te ver van de wand?
        State = 92;      
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





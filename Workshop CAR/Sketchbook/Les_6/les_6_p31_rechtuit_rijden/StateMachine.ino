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
        // 200 ticks voor 2 wielen is ongeveer 1 meter per wiel
        Doel = EncoderLTeller + EncoderRTeller + 200;  
        Richting = EncoderLTeller - EncoderRTeller;
      }
      // Regel motoren op basis van setpoint 'Richting'
      int Afwijking = EncoderLTeller - EncoderRTeller - Richting;
      int Correctie = Afwijking * -10; // versterking
      if (Correctie > 100)  Correctie = 100;
      if (Correctie < -100) Correctie = -100;
      Motors(115 + Correctie, 115 - Correctie);

      printf("State5  L/R: %d %d, Richting: %d, Afwijking: %d\n", 
          EncoderLTeller, EncoderRTeller, Richting, Afwijking);

      // Conditie
      if ((EncoderLTeller + EncoderRTeller - Doel) > 0) {  
        // ver genoeg gereden!
        State = 10;
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





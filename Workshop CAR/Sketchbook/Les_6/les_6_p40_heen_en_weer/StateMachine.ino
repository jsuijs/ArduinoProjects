// StateMachine.ino - 

//-----------------------------------------------------------------------------
// HeenWeerTakt - State machine voor aansturing motor.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void HeenWeerTakt()
{ 
  static int State, PrevState = -1;
  bool NewState = false;
  static int Richting, Doel;

  // rapporteer status bij state overgang
  if (PrevState != State) {  
    PrevState = State;  
    printf("HeenWeerTakt State: %d\n", State);
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
        Doel = EncoderLTeller + EncoderRTeller + 200;
        Richting = EncoderLTeller - EncoderRTeller;
      }
      // Regel motoren zodat de rijrichting gelijk blijft aan de startrichting
      int Afwijking = EncoderLTeller - EncoderRTeller - Richting;
      int Correctie = Afwijking * -10; // versterking
      if (Correctie > 100)  Correctie = 100;
      if (Correctie < -100) Correctie = -100;
      Motors(115 + Correctie, 115 - Correctie);

      printf("S5 L/R: %d %d, Richting: %d, Afwijking: %d\n", EncoderLTeller, EncoderRTeller, Richting, Afwijking);
      
      // Conditie
      if ((EncoderLTeller + EncoderRTeller - Doel) > 0) {  // ver genoeg gereden?
        State = 10;
      }
      break;
    }

  case 10 :       // State actie : draai 180 graden linksom
    {   
      if (NewState) {
        // bepaal nieuwe rijrichting
        Richting = EncoderLTeller - EncoderRTeller - 40;
      }

      Motors(0, 120); // alleen rechtse motor draait

      printf("S10 L/R: %d %d, Richting: %d, Afwijking: %d \n", EncoderLTeller, EncoderRTeller, Richting, EncoderLTeller - EncoderRTeller - Richting);

      // Conditie
      if ((EncoderLTeller - EncoderRTeller - Richting) < 0) { // voldoende gedraaid?
        State = 15; // rij terug
      }      
      break;
    }

  case 15 :       // State actie : Rij terug (op odometrie)
    {   
      if (NewState) {
        // bewaar rijrichting
        Doel = EncoderLTeller + EncoderRTeller + 200;
      }

      // Regel motoren zodat de rijrichting gelijk blijft aan de startrichting
      int Afwijking = EncoderLTeller - EncoderRTeller - Richting;
      int Correctie = Afwijking * -10; // versterking
      if (Correctie > 100)  Correctie = 100;
      if (Correctie < -100) Correctie = -100;
      Motors(115 + Correctie, 115 - Correctie);

      printf("S15 L/R: %d %d, Richting: %d, Afwijking: %d\n", EncoderLTeller, EncoderRTeller, Richting, Afwijking);
      
      // Conditie
      if ((EncoderLTeller + EncoderRTeller - Doel) > 0) { // voldoende gereden?
        State = 0; // we zijn klaar
      }      
      break;
    }

  default : 
    {
      printf("HeenWeerTakt: ongeldige state %d\n", State);
      State = 0;
      break;
    }
  } // einde van switch 
}






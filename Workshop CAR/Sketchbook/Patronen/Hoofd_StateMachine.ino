
int Teller;

void StateMachineTakt()
{ 
  static int State, PrevState = -1;
  bool NewState = false;

  // rapporteer status bij state overgang
  if (PrevState != State) {  
    PrevState = State;  
    printf("StateMachineTakt State: %d\n", State);
    NewState = true;
  }

  // state machine
  switch(State) {
  case 0 : // State: stil staan
    {  
      if (NewState) {
        // OnEntry actie
        Teller = 10;
      }

      // State actie
      Teller--;
      digitalWrite(13, true); // led aan

        // Condities
      if (Teller <= 0) { // Tijd voorbij?
        digitalWrite(13, false); // led uit
        State = 1; // wandvolgen
      }
      break;
    }

  default : 
    {
      printf("StateMachineTakt: ongeldige state %d\n", State);
      State = 0;
      break;
    }
  } // einde van switch 
}


void StateMachineTakt()
{ 
  static int State, PrevState = -1;
  bool NewState = false;

  // rapporteer status bij state overgang
  if (PrevState != State) {  
    PrevState = State;  
    printf("StateMachineTakt State: %d\n", State);
    NewState = true;
  }

  // state machine
  switch(State) {
  case 0 : // State: stil staan
    {
      ...
      break;
    }

  case 1 : // State: wandvolgen
    {
      ...
      break;
    }

  default : 
    {
      printf("StateMachineTakt: ongeldige state %d\n", State);
      State = 0;
      break;
    }
  } // einde van switch 
}

*/

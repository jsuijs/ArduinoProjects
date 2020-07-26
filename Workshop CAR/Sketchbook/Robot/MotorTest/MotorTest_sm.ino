// MotorTest_sm.ino - Motor Test Statemachine


//-----------------------------------------------------------------------------
// MotorTestTakt - State machine voor testen aansturing motor.
//-----------------------------------------------------------------------------
// * Led flitst kort in rust
// * Led bijna continue aan tijdens testen
// * druk op KNOP selecteert volgende test
// * Test eerst rechtse motor vooruit (3x), dan rechts achteruit (3x)
// * aansluitend dezelfde reeks voor linkse motor 
//-----------------------------------------------------------------------------
void MotorTestTakt()
{ 
  static int State, PrevState = -1;
  bool NewState = false;

  // rapporteer status bij state overgang
  if (PrevState != State) {  
    PrevState = State;  
    printf("MotorTestTakt State: %d\n", State);
    NewState = true;
  }

  // state machine
  switch(State) {
  case 0 : // State: rust
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motors in rust\n");
      }

      // State actie
      Motors(0,0);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 1 : // State: rechts langzaaam vooruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor rechts langzaam vooruit\n");
      }

      // State actie
      Motors(0,LOW_PWM);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 2 : // State: rechts snel vooruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor rechts snel vooruit\n");
      }

      // State actie
      Motors(0,HIGH_PWM);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 3 : // State: rechts langzaaam vooruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor rechts langzaam vooruit\n");
      }

      // State actie
      Motors(0,LOW_PWM);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 4 : // State: rechts langzaaam achteruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor rechts langzaam achteruit\n");
      }

      // State actie
      Motors(0,-LOW_PWM);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 5 : // State: rechts snel achteruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor rechts snel achteruit\n");
      }

      // State actie
      Motors(0,-HIGH_PWM);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 6 : // State: rechts langzaaam achteruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor rechts langzaam achteruit\n");
      }

      // State actie
      Motors(0,-LOW_PWM);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }                        

  case 7 : // State: links langzaaam vooruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor links langzaam vooruit\n");
      }

      // State actie
      Motors(LOW_PWM, 0);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 8 : // State: links snel vooruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor links snel vooruit\n");
      }

      // State actie
      Motors(HIGH_PWM, 0);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 9 : // State: links langzaaam vooruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor links langzaam vooruit\n");
      }

      // State actie
      Motors(LOW_PWM, 0);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 10 : // State: links langzaaam achteruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor links langzaam achteruit\n");
      }

      // State actie
      Motors(-LOW_PWM, 0);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 11 : // State: links snel achteruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor links snel achteruit\n");
      }

      // State actie
      Motors(-HIGH_PWM, 0);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 12 : // State: links langzaaam achteruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor links langzaam achteruit\n");
      }

      // State actie
      Motors(-LOW_PWM, 0);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State = 0; // einde serie, naar rust state
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

  // signaleer rust / actief op LED
  if (State == 0) {
    // Rust
    FlashLed(10); // 10% aan
  } 
  else {
    FlashLed(90); // 90% aan
  }
}



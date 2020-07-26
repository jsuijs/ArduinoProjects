// MotorTest_Rev3_sm.ino - Motor Test Statemachine voor Rev3 motorshield


//-----------------------------------------------------------------------------
// MotorTestTakt - State machine voor testen aansturing motor.
//-----------------------------------------------------------------------------
// * Led flitst kort in rust
// * Led bijna continue aan tijdens testen
// * druk op KNOP selecteert volgende test
// * De testen:
//    rechtse motor vooruit
//    rechtse motor achteruit
//    linkse motor vooruit
//    linkse motor achteruit
//    rust
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

  case 1 : // State: rechts vooruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor rechts vooruit\n");
      }

      // State actie
      Motors(0,MOTOR_PWM);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 2 : // State: rechts achteruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor rechts achteruit\n");
      }

      // State actie
      Motors(0,-MOTOR_PWM);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 3 : // State: links vooruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor links vooruit\n");
      }

      // State actie
      Motors(MOTOR_PWM, 0);

      // Condities
      if (KnopFlank()) { // Knop ingedrukt
        State ++; // volgende state
      }
      break;
    }

  case 4 : // State: links achteruit
    {  
      if (NewState) {
        // OnEntry actie
        printf("Motor links achteruit\n");
      }

      // State actie
      Motors(-MOTOR_PWM, 0);

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



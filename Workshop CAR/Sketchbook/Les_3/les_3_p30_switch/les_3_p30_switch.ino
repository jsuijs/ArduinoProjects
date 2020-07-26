void setup() {                
  pinMode(13, OUTPUT);        // led output
  pinMode(11, INPUT_PULLUP);  // knop input & pull-up

  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

int State = 1;

void loop() {
  delay(10);
  
  switch(State) {
    case 1 : {  // wacht op knop los
      if (digitalRead(11) == HIGH) {
        State = 2;
      }
      break;
    }
    case 2 : {  // wacht op knop ingedrukt
      if (digitalRead(11) == LOW) {
        digitalWrite(13, HIGH);            // LED aan        
        State = 3;
      }
      break;
    }    
    case 3 : {  // wacht op knop los
      if (digitalRead(11) == HIGH) {
        State = 4;
      }
      break;
    }
    case 4 : {  // wacht op knop ingedrukt
      if (digitalRead(11) == LOW) {
        digitalWrite(13, LOW);            // LED uit
        State = 1;
      }
      break;
    }
    
    default : {
      State = 1;
      break;
    }
  } // einde van switch
}


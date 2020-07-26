void setup() {                
  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

int State = 7;

void loop() {
  delay(100);

  Serial.print("***");

  switch(State) {
    case 0 : {
      Serial.println("Nul ");
      State = 1;
    }
    case 1 : {
      Serial.println("Een ");
      State = 2;
      break;
    }
    case 2 : {
      Serial.println("Twee");
      State = 0;
    }
    default : {
      Serial.println("Ongeldige state");
      State = 1;
      break;
    }
  } // einde van switch
}


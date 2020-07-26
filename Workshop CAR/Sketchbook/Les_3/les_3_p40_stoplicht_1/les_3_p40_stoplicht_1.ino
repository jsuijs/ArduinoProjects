void setup() {              
  pinMode(5, OUTPUT);         // rode   led 
  pinMode(6, OUTPUT);         // gele   led 
  pinMode(7, OUTPUT);         // groene led 
  
  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

void loop() {
  delay(100);

  StoplichtTakt();
}

void StoplichtTakt()
{ static int State = 0;

  switch(State) {
    case 0 : {  // rood
      // rood              geel                  groen
      digitalWrite(5, 1);  digitalWrite(6, 0);   digitalWrite(7, 0);  
      State = 1;
      break;
    }

    default : {
      Serial.print("Ongeldige state ");
      Serial.println(State);
      State = 0;
      break;
    }
  } // einde van switch
}


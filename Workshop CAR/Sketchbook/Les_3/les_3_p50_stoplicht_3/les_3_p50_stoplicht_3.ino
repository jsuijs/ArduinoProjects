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
  static int Tijd = 0;
 
  Tijd = Tijd + 1;
  Serial.println(Tijd);

  switch(State) {
    case 0 : {  // rood licht
      // rood              geel                  groen
      digitalWrite(5, 1);  digitalWrite(6, 0);   digitalWrite(7, 0);  
      
      if (Tijd > 20) {
        Tijd = 0;
        State = 1;
      }
      break;
    }
    case 1 : {  // groen licht
      // rood              geel                  groen
      digitalWrite(5, 0);  digitalWrite(6, 0);   digitalWrite(7, 1);  
      if (Tijd > 20) {
        Tijd = 0;
        State = 2;
      }
      break;
    }
    case 2 : {  // geel / oranje licht
      // rood              geel                  groen
      digitalWrite(5, 0);  digitalWrite(6, 1);   digitalWrite(7, 0);  
      if (Tijd > 5) {
        Tijd = 0;      
        State = 0;
      }
      break;
    }
    default : {
      Serial.print("Ongeldige state ");
      Serial.println(State);
      Tijd = 0;
      State = 0;
      break;
    }
  } // einde van switch
}


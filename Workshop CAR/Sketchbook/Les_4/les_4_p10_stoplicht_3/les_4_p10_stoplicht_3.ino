void setup() {              
  pinMode(5, OUTPUT);         // rode   led 
  pinMode(6, OUTPUT);         // gele   led 
  pinMode(7, OUTPUT);         // groene led 

  pinMode(11,  INPUT_PULLUP);  // knop input & pull-up
  
  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

int Toestand = 0;
int Tijd = 0;

void loop() {
  delay(100);
  Tijd = Tijd + 1;
  Serial.println(Tijd);

  switch(Toestand) {
    case 0 : {  // rood licht
      // rood              geel                  groen
      digitalWrite(5, 1);  digitalWrite(6, 0);   digitalWrite(7, 0);  
      
      if (Tijd > 20) {
        Tijd = 0;
        Toestand = 1;
      }
      if (digitalRead(11) == 0) { // knop ingedrukt?
        Tijd = 0;
        Toestand = 1;      
      }
      
      break;
    }
    case 1 : {  // groen licht
      // rood              geel                  groen
      digitalWrite(5, 0);  digitalWrite(6, 0);   digitalWrite(7, 1);  
      if (Tijd > 20) {
        Tijd = 0;
        Toestand = 2;
      }
      break;
    }
    case 2 : {  // geel / oranje licht
      // rood              geel                  groen
      digitalWrite(5, 0);  digitalWrite(6, 1);   digitalWrite(7, 0);  
      if (Tijd > 5) {
        Tijd = 0;      
        Toestand = 0;
      }
      break;
    }
    default : {
      Serial.println("Ongeldige state");
      Tijd = 0;
      Toestand = 0;
      break;
    }
  } // einde van switch
}


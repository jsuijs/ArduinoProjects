void setup() {                
  pinMode(13, OUTPUT);         // led output
  pinMode(11,  INPUT_PULLUP);  // knop input & pull-up

  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

int State = 1;
void loop() {
  delay(10);

  Serial.println(State);

  if (State == 1) {
    if (digitalRead(11) == HIGH) { 
      State = 2;             // ga naar state 2 als knop hoog is.
    }    
  }
  if (State == 2) {
    if (digitalRead(11) == LOW) { 
      State = 3;             // ga naar state 3 als knop laag is.
      digitalWrite(13, 1);   // en zet de led aan
    }    
  }


}






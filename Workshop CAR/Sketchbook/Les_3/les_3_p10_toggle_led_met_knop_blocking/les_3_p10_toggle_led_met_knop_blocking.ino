void setup() {                
  pinMode(13, OUTPUT);         // led output
  pinMode(11,  INPUT_PULLUP);  // knop input & pull-up
}

void loop() {
  
  while (digitalRead(11) != HIGH) {} // wacht op knop los

  while (digitalRead(11) != LOW) {}  // wacht op knop ingedrukt

  digitalWrite(13, HIGH);            // LED aan
  
  while (digitalRead(11) != HIGH) {} // wacht op knop los

  while (digitalRead(11) != LOW) {}  // wacht op knop ingedrukt

  digitalWrite(13, LOW);             // LED uit
  
}




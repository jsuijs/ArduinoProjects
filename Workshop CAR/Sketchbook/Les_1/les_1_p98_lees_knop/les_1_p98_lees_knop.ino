void setup() {                
  pinMode(13, OUTPUT);         // led output
  pinMode(11, INPUT_PULLUP);   // knop input & pull-up
  
  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

void loop() {
  int Knop;

  Knop = digitalRead(11);
  Serial.println(Knop);

  if (Knop) {  
    digitalWrite(13, true);
  } else {
    digitalWrite(13, false);
  }  
}




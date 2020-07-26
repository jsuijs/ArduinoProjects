void setup() {                
  pinMode(13, OUTPUT);         // led output
  pinMode(11, INPUT_PULLUP);   // knop input & pull-up
  
  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

int AanTijd = 0;

void loop() {
  int Knop;
  delay(10);  
  
  Knop = digitalRead(11);
  Serial.println(Knop);

  if (Knop == 0) {  
    AanTijd = 100;
  }
  
  if (AanTijd > 0) {
    AanTijd = AanTijd - 1;
    digitalWrite(13, true);
  } else {
    digitalWrite(13, false);
  }  
}




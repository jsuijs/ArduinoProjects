void setup() {                
  pinMode(13, OUTPUT);         // led output
  pinMode(11,  INPUT_PULLUP);  // knop input & pull-up
  
  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

void wacht()
{ int Knop;

  Knop = digitalRead(11);

  if (Knop) {  
    delay(500);
  } else {
    delay(100);
  }  
}

void loop() {
  digitalWrite(13, HIGH);
  wacht();           
  digitalWrite(13, LOW); 
  wacht();        
}




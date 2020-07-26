void setup() {                
  pinMode(13, OUTPUT);         // led output
  pinMode(11,  INPUT_PULLUP);  // knop input & pull-up

  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

void loop() {
  delay(100);

  static bool VorigeKnop;
  bool Knop = digitalRead(11);
  if (Knop != VorigeKnop) {
    // flank (knop ingedrukt of losgelaten)
    VorigeKnop = Knop;  
    if (Knop == 0) {
      // knop is nu ingedrukt
      Serial.println("De knop is ingedrukt");      
      ToggleLed();
    }
  }
}

void ToggleLed()
{
  if (digitalRead(13)) {
    // led is aan, dus nu uitzetten
    digitalWrite(13, 0);
  } 
  else {
    // led is uit, dus nu aanzetten
    digitalWrite(13, 1);
  }
}





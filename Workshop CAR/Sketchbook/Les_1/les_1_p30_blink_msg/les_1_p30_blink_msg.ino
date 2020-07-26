void setup() {                
  pinMode(13, OUTPUT);     

  // start serial
  Serial.begin(115200); // This sets up a serial connection, 115.2 kbps.
  Serial.println("Hello World");  
}

void loop() {
  digitalWrite(13, HIGH);
  delay(100);           
  digitalWrite(13, LOW); 
  delay(100);           
}


void setup() {                
  pinMode(13, OUTPUT);     
}

void wacht() {
  delay(100);              
}

void loop() {
  digitalWrite(13, HIGH);  
  wacht();
  digitalWrite(13, LOW);   
  wacht();
}

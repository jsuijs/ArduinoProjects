void setup() {              
  pinMode(5, OUTPUT);         // rode   led 
  pinMode(6, OUTPUT);         // gele   led 
  pinMode(7, OUTPUT);         // groene led 
  
  // start serial
  Serial.begin(115200); 
  Serial.println("Hallo arduino wereld.");
}

int Toestand = 0;

void loop() {

  digitalWrite(5, 1);  // rood
  delay(300);
  
  digitalWrite(6, 1);  // geel
  delay(300);
  
  digitalWrite(7, 1);  // groen
  delay(300);
  
  digitalWrite(5, 0);  // rood
  delay(300);
  
  digitalWrite(6, 0);  // geel
  delay(300);
  
  digitalWrite(7, 0);  // groen
  delay(300);
  
}


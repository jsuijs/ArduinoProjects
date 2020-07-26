void setup() {                
  Serial.begin(115200); 
  
  int t;
  t = 29 / 10;
  Serial.println(t);   
  
  t = (29 / 10) * 5;
  Serial.println(t);   
  
  t = 255 * 255;
  Serial.println(t);   
}

void loop() {
}




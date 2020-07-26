void setup() {                
  Serial.begin(115200);   
}

int t = 0;

void loop() { 
  t = t + 1;
  Serial.println(t);
}




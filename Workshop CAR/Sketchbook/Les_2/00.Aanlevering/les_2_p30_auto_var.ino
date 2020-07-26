void setup() {                

  // start serial
  Serial.begin(115200); 
  test();
  test();
  test();
}

void test() {
  int teller;
  teller = teller + 1;
  Serial.println(teller);            
}

void loop() {
}




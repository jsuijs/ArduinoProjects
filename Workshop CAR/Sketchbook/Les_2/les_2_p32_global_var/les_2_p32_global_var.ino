void setup() {                

  // start serial
  Serial.begin(115200); 
  test();
  test();
  test();
}

int teller = 3;

void test() {
  teller = teller + 1;
  Serial.println(teller);            
}

void loop() {
}




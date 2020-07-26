const int KNOP = 2;

// to support printf 
int my_putc(char c, FILE *t) {  
  return Serial.write(c);
}

void setup() {                
  Serial.begin(115200);           // start serial
  fdevopen( &my_putc, 0);        // device 0 (stdout) output naar my_putc()
  pinMode(KNOP,  INPUT_PULLUP);  // knop input & pull-up

  SetupMotors();
  printf("Opstarten gereed.\n");  
}

void loop() {
  delay(10);
  
  if (digitalRead(KNOP) == 0) {
    Motors(200, -200);
  }    
}



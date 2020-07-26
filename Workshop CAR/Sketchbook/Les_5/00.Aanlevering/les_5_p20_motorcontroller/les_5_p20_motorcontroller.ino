
const int PWML = 11;
const int PWMR = 3;

const int DIRL = 13;
const int DIRR = 12;

const int KNOP = 2;

// to support printf 
int my_putc(char c, FILE *t) {  
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(115200);           // start serial
  fdevopen( &my_putc, 0);        // device 0 (stdout) output naar my_putc()
  pinMode(KNOP,  INPUT_PULLUP);  // knop input & pull-up

  // Break gebruiken we verder niet...
  pinMode( 8, OUTPUT);         // brakeB
  digitalWrite(8, 0);
  pinMode( 9, OUTPUT);         // brakeA
  digitalWrite(9, 0);

  // Rijrichting
  pinMode(DIRL, OUTPUT);       
  pinMode(DIRR, OUTPUT);        
  digitalWrite(DIRL, 0);  // vooruit
  digitalWrite(DIRR, 0);  // vooruit
  
  // duty cycle
  analogWrite(PWML, 0);    // uit, 0 of 150..255 
  analogWrite(PWMR, 0);    // uit

  printf("Opstarten gereed.\n");  
}

void loop() {
  delay(10);

  if (digitalRead(KNOP) == 0) {
    // hier de motor code
  }    
}



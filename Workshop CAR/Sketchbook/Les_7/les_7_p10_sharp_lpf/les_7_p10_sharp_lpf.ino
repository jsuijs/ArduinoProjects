
// to support printf 
int my_putc(char c, FILE *t) {  
  if (c == '\n') Serial.write('\r');
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(115200);           // start serial
  fdevopen( &my_putc, 0);        // device 0 (stdout) output naar my_putc()

  printf("Opstarten gereed.\n");  
}

int Centimeters;
int LpfSharpR;

void loop() {
  static int SecTakt;
     
  // seconde lus
 int  ms = millis();
  if ((ms - SecTakt) > 0) {
    SecTakt = ms + 1000;  // zet tijd voor volgende interval 
    
    Centimeters = SharpAfstand(A2, LpfSharpR);    
    printf("Sharp: %d cm\n", Centimeters);
  } 
}




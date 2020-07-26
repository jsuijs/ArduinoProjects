
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
  
  int A = 41;
  int B = 18;
  int C = 27;
  
  printf("%d &  %d = %d\n", A, B, A &  B);
  printf("%d && %d = %d\n", A, B, A && B);
  printf("%d &  %d = %d\n", A, C, A &  C);
  printf("%d && %d = %d\n", A, C, A && C);
  
}


void loop() {
}

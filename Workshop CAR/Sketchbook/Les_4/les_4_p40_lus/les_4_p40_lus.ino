
// to support printf 
int my_putc(char c, FILE *t) {  
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {                
  // start serial
  Serial.begin(115200); 

  fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()

  printf("Opstarten gereed.\n");  
}

void loop() {
  delay(1000);
  
  int teller = 0;         // initialisatie_expressie
lus:
  if (teller < 10) {	  // loop_condition
    printf("Teller is %d\n", teller);

    teller = teller + 1;  // increment_expression
    goto lus;
  }
}



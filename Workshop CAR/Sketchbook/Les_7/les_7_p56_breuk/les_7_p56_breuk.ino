// to support printf 
int my_putc(char c, FILE *t) {  
  if (c == '\n') Serial.write('\r');
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(115200);           // start serial
  fdevopen( &my_putc, 0);        // device 0 (stdout) output naar my_putc()

  //  5,2857142857142857142857142857143

  printf("Opstarten gereed.\n");  

  PrintBreuk(37, 7, 30);
}

void PrintBreuk(int Teller, int Noemer, int Decimalen)
{ 
  int Quotient;
  int Rest = Teller;
  bool First = true;  
  
  Decimalen += 2; 
  
  // Staartdeling
  while (Decimalen) {
    Quotient = Rest / Noemer;
    Rest = (Rest - Quotient * Noemer) * 10; 
    printf("%d", Quotient); 
    if (First) {
      // Na de eerste keer de decimale punt printen.
      printf(".");
      First = false;
    }
    Decimalen --;
  }
}

void loop() {
}


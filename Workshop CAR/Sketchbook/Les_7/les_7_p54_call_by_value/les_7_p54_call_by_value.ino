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

int Teller;

void loop() {
  TestFunctie(Teller);
}

void TestFunctie(int Teller)
{
  Teller++;
  printf("Teller: %d\n", Teller); 
}


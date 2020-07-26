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
  
  int test = 5;
  
  if (test = 0)  printf("Nul\n");
  if (test = 1)  printf("Een\n");
  if (test = 2)  printf("Twee\n");
  if (test = 3)  printf("Drie\n");
  if (test = 4)  printf("Vier\n");
  if (test = 5)  printf("Vijf\n");
  if (test = 6)  printf("Zes\n");

}

void loop() {
}

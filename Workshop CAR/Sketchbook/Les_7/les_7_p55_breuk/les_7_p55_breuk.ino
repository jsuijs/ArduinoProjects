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

  a(37, 7, 30);
}

void a(int b, int c, int d)
{ int e, f = 1; d += 2; 
  
  while (d--) {
    e = b / c;  b = (b - e * c) * 10; 
    printf("%d", e); if (f) printf("."); 
    f = 0;
  }
}

void loop() {
}

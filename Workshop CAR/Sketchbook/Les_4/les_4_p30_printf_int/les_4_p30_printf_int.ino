
// to support printf 
int my_putc(char c, FILE *t) {  
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {                
  // start serial
  Serial.begin(115200); 

  fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()

  printf("Opstarten gereed.\nJa, echt!\n");  
}

int Seconde;
void loop() {
  delay(1000);
  Seconde = Seconde + 1;
  
  printf("Er zijn %d seconden verstreken sinds reset.\n", Seconde);
}



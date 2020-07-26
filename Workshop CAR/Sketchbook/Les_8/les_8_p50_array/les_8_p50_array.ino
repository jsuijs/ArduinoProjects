

int Waarden[12];  // array met positie 0 t/m 11

// to support printf 
int my_putc(char c, FILE *t) {  
  if (c == '\n') Serial.write('\r');  
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {
  // start serial
  Serial.begin(115200);

  fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()

  for (int i=0; i<12; i++) {
    Waarden[i] = 0;
  }
  PrintWaarden();

  Waarden[3] = 7;

  Waarden[4] = Waarden [3] + 2;

  PrintWaarden();

  for (int i=0; i<12; i++) {
    Waarden[i]++;
  }

  PrintWaarden();
}

void loop()
{
}



void PrintWaarden()
{
  // uitgeschreven
  printf("Array waarden: %d %d %d %d %d %d %d %d %d %d %d %d\n",
    Waarden[0], Waarden[1], Waarden[2], Waarden[3], 
    Waarden[4], Waarden[5], Waarden[6], Waarden[7],
    Waarden[8], Waarden[9], Waarden[10], Waarden[11]);

   // alternatief: in lus
  for (int i=0; i<12; i++) {
    printf("Waarde[%d] = %d\n", i, Waarden[i]);
  }
}


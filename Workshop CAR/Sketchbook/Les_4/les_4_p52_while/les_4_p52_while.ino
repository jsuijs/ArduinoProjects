
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

  int teller = 0;
  while (teller < 6) {
    teller = teller + 1;
    printf("Teller is %d\n", teller);
    if (teller == 1) continue;
    printf("###\n");
    if (teller == 4) break;
    printf("***\n");
  }


  delay(5000);
}




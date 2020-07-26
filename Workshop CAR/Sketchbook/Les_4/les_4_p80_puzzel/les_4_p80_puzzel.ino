int my_putc(char c, FILE *t) {  
  return Serial.write(c);
}

void setup() {                
  Serial.begin(115200); 
  fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()
  printf("Opstarten gereed.\n");  
  
  printf("* %d *\n", 123);
  printf("* %6d *\n", 123);
  printf("* %06d *\n", 123);
  printf("* %-6d *\n", 123);
  printf("* 0x%02x *\n", 123);
  printf("* 0x%02X *\n", 123);
  
  int x = 7;
  while (x) { 
    printf("x: %d\n", x);
    x--;
  }
  
  int y = -3;
  while (y<100) { 
    printf("y: %d\n", y);
    y += 7;
  }
  
  while (1) {
    delay(1000);
    printf("lus \"while (1)\"\n");    
  }
}

void loop() {
  delay(1000);
  printf("Hoofdlus.\n");  
}


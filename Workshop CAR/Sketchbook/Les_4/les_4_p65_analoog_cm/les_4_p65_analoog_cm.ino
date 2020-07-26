
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

  int SensorValue = analogRead(A2);   
  int Afstand = (40*148) / SensorValue;
  
  printf("Sensor input is %d, Afstand: %d\n", SensorValue, Afstand);
}



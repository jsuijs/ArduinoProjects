
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

  int Centimeters = SharpAfstand(A2);  
  printf("Sensor Afstand: %d\n", Centimeters);
}

int SharpAfstand(int Pin)
{
  int SensorValue = analogRead(Pin);
  int Afstand = (40*148) / SensorValue;
  return Afstand;
}

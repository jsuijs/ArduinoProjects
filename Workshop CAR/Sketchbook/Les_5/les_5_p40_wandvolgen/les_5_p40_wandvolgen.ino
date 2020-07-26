
#define KNOP 2
int Centimeters;

// to support printf 
int my_putc(char c, FILE *t) {  
  if (c == '\n') Serial.write('\r');
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(115200);           // start serial
  fdevopen( &my_putc, 0);        // device 0 (stdout) output naar my_putc()
  pinMode(KNOP,  INPUT_PULLUP);  // knop input & pull-up

  SetupMotors();
  printf("Opstarten gereed.\n");  
}

void loop() {
  delay(10);

  Centimeters = SharpAfstand(A2);    
  printf("Sharp afstand is %d\n", Centimeters);

  MotorTakt();  
}

int SharpAfstand(int Pin)
{
  int SensorValue = analogRead(Pin);
  int Afstand = (40*148) / SensorValue;
  return Afstand;
}



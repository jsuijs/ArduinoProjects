
const int KNOP           = 2 ;
const int LED            = 5 ;
int Centimeters;

// to support printf 
int my_putc(char c, FILE *t) {  
  if (c == '\n') Serial.write('\r');
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(115200);          // start serial
  fdevopen( &my_putc, 0);        // device 0 (stdout) output naar my_putc()
  pinMode(KNOP,  INPUT_PULLUP);  // knop input & pull-up
  pinMode(LED, OUTPUT);          // led pin op output

  SetupMotors();
  printf("Opstarten gereed.\n");  
}

void loop() {
  static int MsTakt, MainTakt, SecTakt;

  // zo vaak mogelijk
  // (nu nog niets te doen)

  // 1 ms lus
  int us = micros();
  if ((us - MsTakt) > 0) {
    MsTakt = us + 1000;  // zet tijd voor volgende interval 
    LedTakt();
    Centimeters = SharpAfstand(A2);    
  }

  // 10 ms lus
  int ms = millis();
  if ((ms - MainTakt) > 0) {
    MainTakt = ms + 10;  // zet tijd voor volgende interval 

    RijdenTakt();  
  }                

  // seconde lus
  ms = millis();
  if ((ms - SecTakt) > 0) {
    SecTakt = ms + 1000;  // zet tijd voor volgende interval 

    printf("Sharp: %d cm\n", Centimeters);
  }    
}

int SharpAfstand(int Pin)
{
  int SensorValue = analogRead(Pin);
  int Afstand = (40*148) / SensorValue;
  return Afstand;
}

void LedTakt()
{ 
  static int Teller;

  Teller++;
  if (Teller > 200) Teller = 0;

  if (Teller > 100) {
    digitalWrite(LED, 1);  // aan
  } 
  else {
    digitalWrite(LED, 0);  // uit
  }
}



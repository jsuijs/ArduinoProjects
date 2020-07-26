
const int KNOP           =  2 ;
const int ENCODER_L_PIN  =  4 ;
const int ENCODER_R_PIN  = A3 ;

int EncoderLTeller, EncoderRTeller;

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
  static int MsTakt, MainTakt, SecTakt;

  // zo vaak mogelijk
  EncoderTakt();
  
  // 1 ms lus
  int us = micros();
  if ((us - MsTakt) > 0) {
    MsTakt = us + 1000;  // zet tijd voor volgende interval 

  }

  // 10 ms lus
  int ms = millis();
  if ((ms - MainTakt) > 0) {
    MainTakt = ms + 10;  // zet tijd voor volgende interval 
    
    HeenWeerTakt();  
  }
  
  // seconde lus
  ms = millis();
  if ((ms - SecTakt) > 0) {
    SecTakt = ms + 1000;  // zet tijd voor volgende interval 
    
    printf("Encoder L/R: %d %d\n", EncoderLTeller, EncoderRTeller);
  }  
}



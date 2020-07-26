
// Encoder variabelen.
// 1 tick = 10,6 mm.
volatile int EncoderLTeller, EncoderRTeller;                              // aantal flanken

#define ENCODER_L_PIN 4
#define ENCODER_R_PIN A3

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

   EncoderSetup();

   printf("\n\nOpstarten gereed.\n");          

}

void loop() {
  static int NextSecTakt;
  int L, R;
  // Seconde interval
  int ms = millis();
  if ((ms - NextSecTakt) > 0) {
    NextSecTakt = ms + 1000;  // zet tijd voor volgende interval 

    EncoderRead(L, R);
    printf("Encoders L/R: %d / %d\n", L, R);    
  } 
}



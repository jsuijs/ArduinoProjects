
// Encoder variabelen.
// 1 tick = 10,6 mm.
volatile int EncoderLTeller, EncoderRTeller;                              // aantal flanken

#define ODO_TICK_TO_METRIC     43418L  // afstand per tick, voor het linkse wiel, 4096 betekent dat 1 tick => 1 mm 

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

  // Seconde interval
  int ms = millis();
  if ((ms - NextSecTakt) > 0) {
    NextSecTakt = ms + 1000;  // zet tijd voor volgende interval 

    EncoderPrint();
    
  } 
}



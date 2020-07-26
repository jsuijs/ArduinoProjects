
// to support printf 
int my_putc(char c, FILE __attribute__ ((unused)) *t) {
  if (c == '\n') Serial.write('\r');  
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {
   // start serial
   Serial.begin(115200);

   fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()

   //  pinMode(KNOP,  INPUT_PULLUP);  // knop input & pull-up

   // Leds
   pinMode(5, OUTPUT);  // Rood
   pinMode(6, OUTPUT);  // Geel
   pinMode(7, OUTPUT);  // Groen

   MotorsSetup();

   EncoderSetup();

  // Link PinChange interrupt to RC5 reader.
  attachInterrupt(RC5_INTERRUPT, Rc5Isr, CHANGE); 

   printf("Opstarten gereed.\n");
}

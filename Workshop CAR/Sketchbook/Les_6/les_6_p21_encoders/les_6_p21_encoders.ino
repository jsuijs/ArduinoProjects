
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

  printf("Opstarten gereed.\n");  
}

void loop() {
  static int SecTakt;
  
  // zo vaak mogelijk
  EncoderTakt(); 
  
  // seconde lus
  int ms = millis();
  if ((ms - SecTakt) > 0) {
    SecTakt = ms + 1000;  // zet tijd voor volgende interval 
    
    printf("Encoder L/R: %d %d\n", EncoderLTeller, EncoderRTeller);
  }   
}

void EncoderTakt()
{  
  static bool VorigeEncoderL;
  static bool VorigeEncoderR;

  // lees linkse encoder    
  bool ActPin = digitalRead(ENCODER_L_PIN);
  if (ActPin != VorigeEncoderL) {
    // flank 
    VorigeEncoderL = ActPin;  
    EncoderLTeller++;
  }

  // lees rechtse encoder    
  ActPin = digitalRead(ENCODER_R_PIN);
  if (ActPin != VorigeEncoderR) {
    // flank 
    VorigeEncoderR = ActPin;  
    EncoderRTeller++;
  }
}



//------------------------------------------------------------------------------
// Encoders.ino - Dit bestand bevat de low level routines voor de encoder.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

volatile int EncoderLTeller, EncoderRTeller;  // aantal flanken

//------------------------------------------------------------------------------
// EncoderSetup -
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void EncoderSetup()
{
  attachInterrupt(ENCODER_L_INTERRUPT, IsrEncoderL, CHANGE); 
  attachInterrupt(ENCODER_R_INTERRUPT, IsrEncoderR, CHANGE); 
}

//------------------------------------------------------------------------------
// EncoderRead - Lees delta van encoders (sinds laatste aanroep)
//------------------------------------------------------------------------------
// Let op: parameters zijn referenties en kunnen dus gebruikt
//         worden om waarden terug te geven.
//------------------------------------------------------------------------------
void EncoderRead (int &LeftDelta, int &RightDelta)
{  int CopyL, CopyR;
   static int LastL, LastR;

  // maak copy zonder dat er interrupts tussendoor komen.
  noInterrupts();
  // critical, time-sensitive code here
  CopyL = EncoderLTeller;  
  CopyR = EncoderRTeller;
  interrupts();
  
  LeftDelta  = CopyL - LastL;
  RightDelta = CopyR - LastR;
  LastL = CopyL;
  LastR = CopyR;
}

//------------------------------------------------------------------------------
// EncoderPrint - print (low level) encoder info
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void EncoderPrint()
{
    printf("Encoder L/R Count: %d/%d\n", EncoderLTeller, EncoderRTeller);
}

//------------------------------------------------------------------------------
// Encoder ISR routines
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void IsrEncoderL()
{
  if (digitalRead(ENCODER_L_PIN_B) == digitalRead(ENCODER_L_PIN_A)) {
    EncoderLTeller++;                            // stapje vooruit
  } else {
    EncoderLTeller--;                            // stapje achteruit
  }  
}

void IsrEncoderR()
{
  if (digitalRead(ENCODER_R_PIN_B) != digitalRead(ENCODER_R_PIN_A)) {
    EncoderRTeller++;                            // stapje vooruit
  } else {
    EncoderRTeller--;                            // stapje achteruit
  }  
}



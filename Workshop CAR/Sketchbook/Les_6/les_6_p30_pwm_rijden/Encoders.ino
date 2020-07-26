//------------------------------------------------------------------------------
// EncoderTakt - Lees encoders uit; zo vaak mogelijk aanroepen.
//------------------------------------------------------------------------------
// Tel pulsen van linkse en rechtse encoder.
// DFROBOT encoders geven 20 pulsen per omwenteling. 
// Wieldiameter 65 mm => 20 cm omtrek => 1 cm/puls
// 
// MaxSpeed: periode ca 10 ms => 
//------------------------------------------------------------------------------
void EncoderTakt()
{  
  static bool VorigeEncoderL;
  static bool VorigeEncoderR;

  // lees linkse encoder    
  bool ActPin = digitalRead(ENCODER_L_PIN);
  if (ActPin != VorigeEncoderL) {
    // flank 
    VorigeEncoderL = ActPin;  
    EncoderLTeller++;                            // stapje gezet
  }

  // lees rechtse encoder    
  ActPin = digitalRead(ENCODER_R_PIN);
  if (ActPin != VorigeEncoderR) {
    // flank 
    VorigeEncoderR = ActPin;  
    EncoderRTeller++;                             // stapje gezet
  }
}

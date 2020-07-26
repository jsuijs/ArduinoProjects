//------------------------------------------------------------------------------
// EncoderInterrupt.ino
//------------------------------------------------------------------------------
// Deze code maakt gebruik van de externe 'library' PinChangeInt.
// Download deze van:
// https://code.google.com/p/arduino-pinchangeint/
// en installeer deze in de 'library' directory van je sketch file.
//
// Zie de installatie instructies voor meer details:
// https://code.google.com/p/arduino-pinchangeint/wiki/Installation
//
//------------------------------------------------------------------------------

// Deze include maakt object 'PCintPort' aan.
#include <PinChangeInt.h>


void IsrEncoderL()
{
  EncoderLTeller++;
}

void IsrEncoderR()
{
  EncoderRTeller++;
}

void EncoderSetup()
{
  pinMode(ENCODER_L_PIN, INPUT_PULLUP); 
  pinMode(ENCODER_R_PIN, INPUT_PULLUP); 

  PCintPort::attachInterrupt(ENCODER_L_PIN, &IsrEncoderL, CHANGE);
  PCintPort::attachInterrupt(ENCODER_R_PIN, &IsrEncoderR, CHANGE);
}

//------------------------------------------------------------------------------
// EncoderRead - Lees de encoders
//------------------------------------------------------------------------------
// Let op: parameters zijn referenties en kunnen dus gebruikt
//         worden om waarden terug te geven.
//------------------------------------------------------------------------------
void EncoderRead (int &Left, int &Right)
{  
  // maak copy zonder dat er interrupts tussendoor komen.
  noInterrupts();
  // critical, time-sensitive code here
  Left = EncoderLTeller;  
  Right = EncoderRTeller;
  interrupts();
}


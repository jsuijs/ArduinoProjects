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

// Encoder Interrupt variabelen
volatile unsigned long LastEncoderLStamp = -10000000;
volatile unsigned long LastEncoderRStamp = -10000000;
volatile unsigned long LastEncoderLPeriode = 10000000;
volatile unsigned long LastEncoderRPeriode = 10000000;

// Deze include maakt object 'PCintPort' aan.
#include <PinChangeInt.h>

// Encoder direction flags
volatile bool EncLeftForward = true;
volatile bool EncLeftChanged;
volatile bool EncRightForward = true;
volatile bool EncRightChanged;

//-----------------------------------------------------------------------------
// InformEncoders - wordt aangeroepen door motors
//-----------------------------------------------------------------------------
// We hebben geen quadratuur encoders, dus gebruiken de pwm om de rijrichting 
// te bepalen. 
//-----------------------------------------------------------------------------
void InformEncoders(int PowerL, int PowerR)
{
   if (PowerL != 0) {
      if (PowerL > 0) {
         // we rijden nu vooruit (tenminste, de pwm is positief...)
         if (EncLeftForward == false) {                            
            // Vorige keer niet vooruit
            EncLeftForward = true;
            EncLeftChanged = true;
         }
      } else {
         // we rijden nu achteruit
         if (EncLeftForward == true) {                            
            // Vorige keer niet achteruit
            EncLeftForward = false;
            EncLeftChanged = true;
         }
      }         
   }
   if (PowerR != 0) {
      if (PowerR > 0) {
         // we rijden nu vooruit (tenminste, de pwm is positief...)
         if (EncRightForward == false) {                            
            // Vorige keer niet vooruit
            EncRightForward = true;
            EncRightChanged = true;
         }
      } else {
         // we rijden nu achteruit
         if (EncRightForward == true) {                            
            // Vorige keer niet achteruit
            EncRightForward = false;
            EncRightChanged = true;
         }
      }         
   }
}

void IsrEncoderL()
{
   unsigned long TmpMicros = micros();

   LastEncoderLPeriode = TmpMicros - LastEncoderLStamp;
   LastEncoderLStamp = TmpMicros;

   if (EncLeftChanged) {
      EncLeftChanged = false;
   } else {
      if (EncLeftForward) {
         EncoderLTeller++;
      } else {
         EncoderLTeller--;
      }
   }
}

void IsrEncoderR()
{
   unsigned long TmpMicros = micros();

   LastEncoderRPeriode = TmpMicros - LastEncoderRStamp;
   LastEncoderRStamp = TmpMicros;

   if (EncRightChanged) {
      EncRightChanged = false;
   } else {
      if (EncRightForward) {
         EncoderRTeller++;
      } else {
         EncoderRTeller--;
      }
   }
}

void EncoderSetup()
{
  pinMode(ENCODER_L_PIN, INPUT_PULLUP); 
  pinMode(ENCODER_R_PIN, INPUT_PULLUP); 

  PCintPort::attachInterrupt(ENCODER_L_PIN, &IsrEncoderL, CHANGE);
  PCintPort::attachInterrupt(ENCODER_R_PIN, &IsrEncoderR, CHANGE);
}

//-------------------------------------------------
// LeftSpeed - actuele snelheid in mm/sec
//-------------------------------------------------
//-------------------------------------------------
int LeftSpeed()
{
  long LastStamp, LastPeriode;
  long tmp, tmp2;

  // schakel interrutps even uit en copieer waarden
  noInterrupts();
  LastStamp  = LastEncoderLStamp;
  LastPeriode = LastEncoderLPeriode;
  interrupts();

  // bereken tijd van lopende periode
  tmp = micros() - LastStamp;  
  tmp2 = tmp;
  
  // langste periodetijd in tmp
  if (tmp < LastPeriode) tmp = LastPeriode;  
   
  // minimale waarde om rare effecten met berekeningen te voorkomen.
  if (tmp < 1000) tmp = 1000;     

  // rijrichting   
  if (!EncLeftForward) tmp *= -1;

  printf("left %ld %ld %ld %ld %d\n", LastStamp, LastPeriode, tmp2, tmp, (int)((ODO_TICK_TO_METRIC*244L) / tmp) );
//left 17034820 427088 220312 -427088 0
      
  return (ODO_TICK_TO_METRIC*244L) / tmp;  // 10.6 mm / tick * 1000_000 microseconde / seconde
}

//-------------------------------------------------
// RightSpeed - actuele snelheid in mm/sec
//-------------------------------------------------
//-------------------------------------------------
int RightSpeed()
{
  long LastStamp, LastPeriode;
  long tmp;

  // schakel interrutps even uit en copieer waarden
  noInterrupts();
  LastStamp  = LastEncoderRStamp;
  LastPeriode = LastEncoderRPeriode;
  interrupts();

  // bereken tijd van lopende periode
  tmp = micros() - LastStamp;  

  // langste periodetijd in tmp
  if (tmp < LastPeriode) tmp = LastPeriode;  

  // minimale waarde om rare effecten met berekeningen te voorkomen.
  if (tmp < 1000) tmp = 1000;                  

  // rijrichting   
  if (!EncRightForward) tmp *= -1;
   
  return (ODO_TICK_TO_METRIC*244L) / tmp;  // 10.6 mm / tick * 1000_000 microseconde / seconde
}

//-------------------------------------------------
// EncoderPrint - 
//-------------------------------------------------
//-------------------------------------------------
void EncoderPrint()
{
//  printf("EncoderInterrupt (%ld %ld %ld %ld) Speed: %d / %d, Count: %d / %d\n", LastEncoderLStamp, LastEncoderRStamp, LastEncoderLPeriode, LastEncoderRPeriode, LeftSpeed(), RightSpeed(), EncoderLTeller, EncoderRTeller);     
  printf("EncoderInterrupt Speed: %d / %d, Count: %d / %d\n", 
      LeftSpeed(), RightSpeed(), EncoderLTeller, EncoderRTeller);     
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


//-----------------------------------------------------------------------------
// Motors_hl.ino  - HL aansturing van de DFRobot motors (hard, push-pull)
//                  Arduino Rev3 motorshield (met doorverbinding)
//-----------------------------------------------------------------------------
// Constantes
// pin 8 is PWM ingang voor motorshield, doorverbinden met pin 10
// !! pin 8 kan niet verder gebruikt worden !!
const int  HL_PWMR = 9;
const int  HL_PWML = 10;  // pub 10 is de PWM uitgang, verbind deze met pin8 - de ingang van het motorshield

const int  HL_DIRL = 13;
const int  HL_DIRR = 12;

const int  HL_ENABLER = 3;
const int  HL_ENABLEL = 11;

//-----------------------------------------------------------------------------
// SetupMotors - Stel IO in voor aansturing motoren (harde aansturing)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SetupMotors()
{
  // enable hebben vaste waarde...
  pinMode(HL_ENABLEL, OUTPUT);     // enable 
  pinMode(HL_ENABLER, OUTPUT);     // enable
  digitalWrite(HL_ENABLEL, 1);
  digitalWrite(HL_ENABLER, 1);

  // Rijrichting
  pinMode(HL_DIRL, OUTPUT);       
  pinMode(HL_DIRR, OUTPUT);        

  // Pwm pins
  pinMode(HL_PWML, OUTPUT);   
  pinMode(HL_PWMR, OUTPUT);   

  // zet output op 0 via Motors()
  Motors(0, 0);
}

//------------------------------------------------------------------------------
// Motors - Stel PWM duty cycle  + rijrichtging in voor beide motoren (harde aansturing)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Motors(int PowerL, int PowerR) 
{

  //--------
  // Motor L 
  //--------

  // begrens waarden
  if (PowerL >  255) { 
    PowerL =  255; 
  }
  if (PowerL < -255) { 
    PowerL = -255; 
  }  

  // Set PWM en richting  
  if (PowerL >= 0) {
    digitalWrite(HL_DIRL, 0);  // vooruit
    analogWrite(HL_PWML, 255-PowerL);    
  } 
  else {
    digitalWrite(HL_DIRL, 1);  // achteruit
    analogWrite(HL_PWML, 255+PowerL);    
  }

  //--------
  // Motor R
  //--------

  // begrens waarden
  if (PowerR >  255) { 
    PowerR =  255; 
  }
  if (PowerR < -255) { 
    PowerR = -255; 
  }

  //Set PWM en richting  
  if (PowerR >= 0) {
    digitalWrite(HL_DIRR, 0);  // vooruit
    analogWrite(HL_PWMR, 255-PowerR);    
  } 
  else {
    digitalWrite(HL_DIRR, 1);  // achteruit
    analogWrite(HL_PWMR, 255+PowerR);    
  }
}


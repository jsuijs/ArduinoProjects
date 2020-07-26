//-----------------------------------------------------------------------------
// Motors_TB6612.ino  - HL aansturing van de RedBot motors (hard, push-pull)
//                    TB6612 chip direct aangesloten
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SetupMotors_HL - Stel IO in voor aansturing motoren (harde aansturing)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MotorsSetup_TB6612()
{
  // Rijrichting
  pinMode(TB6612_IN1L, OUTPUT);       
  pinMode(TB6612_IN2L, OUTPUT);       
  pinMode(TB6612_IN1R, OUTPUT);        
  pinMode(TB6612_IN2R, OUTPUT);        

  // duty cycle
  pinMode(TB6612_PWML, OUTPUT);      
  pinMode(TB6612_PWMR, OUTPUT);      

  // zet output op 0 via Motors()
  Motors(0, 0);
  
//  setPwmFrequency(TB6612_PWML, 4);  // set PWM to 62500/n
//TCCR1B = (TCCR1B & 0b11111000) | 1;  // 2 is delen door 8 - dit is voor pwm op 11/l2, is nu 44/45

   TCCR5B = (TCCR5B & 0b11111000) | 1;  // 2 is delen door 8 - dit is voor pwm op 11/l2, is nu 44/45

}

//------------------------------------------------------------------------------
// Motors_TB6612 - harde aansturing
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Motors_TB6612(int PwmL, int PwmR) 
{
  //  printf("Motors %d %d\n", PwmL, PwmR);
  //--------
  // Motor L 
  //--------

  // begrens waarden  
  PwmL = constrain(PwmL, -255, 255);
  
  // Set PWM en richting  
  if (PwmL > 0) {
    digitalWrite(TB6612_IN1L, 1);  // vooruit
    digitalWrite(TB6612_IN2L, 0);  // vooruit
    analogWrite(TB6612_PWML, PwmL);    
  } 
  else {
    digitalWrite(TB6612_IN1L, 0);  // achteruit
    digitalWrite(TB6612_IN2L, 1);  // achteruit
    analogWrite(TB6612_PWML, -PwmL);    
  }

  //--------
  // Motor R
  //--------

  // begrens waarden
  PwmR = constrain(PwmR, -255, 255);

  // Set PWM en richting  
  if (PwmR > 0) {
    digitalWrite(TB6612_IN1R, 0);  // vooruit
    digitalWrite(TB6612_IN2R, 1);  // vooruit
    analogWrite(TB6612_PWMR, PwmR);    
  } 
  else {
    digitalWrite(TB6612_IN1R, 1);  // achteruit
    digitalWrite(TB6612_IN2R, 0);  // achteruit
    analogWrite(TB6612_PWMR, -PwmR);    
  }
}




//-----------------------------------------------------------------------------
// Motors_TB6612.cpp -
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SetupMotors - Stel IO in voor aansturing motoren
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SetupMotors()
{
  // Rijrichting
  pinMode(TB6612_DIRL_A1, OUTPUT);
  pinMode(TB6612_DIRL_A2, OUTPUT);
  pinMode(TB6612_DIRR_B1, OUTPUT);
  pinMode(TB6612_DIRR_B2, OUTPUT);

  // Pwm pins
  pinMode(TB6612_PWML, OUTPUT);
  pinMode(TB6612_PWMR, OUTPUT);

  analogWriteFrequency(20000); // Set PMW period to 2000 Hz instead of 1000

  // zet output op 0 via Motors()
  Motors(0, 0);
}

//-----------------------------------------------------------------------------
// Motors - Stel PWM duty cycle in voor beide motoren
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Motors(int PwmL, int PwmR)
{
  //*
  //Serial2.print("Motors");
  //  Serial2.print(PwmL);
  //  Serial2.print(" ");
  //  Serial2.println(PwmR);
  //*/
  //--------
  // Motor L
  //--------

  // begrens waarden
  PwmL = constrain(PwmL, -255, 255);

  // Set PWM en richting
  if (PwmL >= 0) {
    digitalWrite(TB6612_DIRL_A1, 0);
    analogWrite( TB6612_DIRL_A2, 255);  // 0 av vooruit
    analogWrite( TB6612_PWML, PwmL);
  }
  else {
    digitalWrite(TB6612_DIRL_A1, 1);
    analogWrite( TB6612_DIRL_A2, 0);  // achteruit
    analogWrite( TB6612_PWML, -PwmL);
  }

  //--------
  // Motor R
  //--------

  // begrens waarden
  PwmR = constrain(PwmR, -255, 255);

  //Set PWM en richting
  if (PwmR >= 0) {
    digitalWrite(TB6612_DIRR_B1, 0);  // 0 av vooruit
    analogWrite( TB6612_DIRR_B2, 255);
    analogWrite( TB6612_PWMR, PwmR);
  }
  else {
    digitalWrite(TB6612_DIRR_B1, 1);  // achteruit
    analogWrite( TB6612_DIRR_B2, 0);
    analogWrite( TB6612_PWMR, -PwmR);
  }
}

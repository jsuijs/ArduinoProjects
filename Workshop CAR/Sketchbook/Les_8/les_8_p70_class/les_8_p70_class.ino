
#include "my.h" // class definities includen
                //  *voor* alle andere code

Sharp SharpL, SharpM, SharpR;

void setup()
{
  SharpL.Pin = A0;
  SharpM.Pin = A1;
  SharpR.Pin = A3;
}

void loop()
{
  SharpL.Sample();
  SharpM.Sample();
  SharpR.Sample();
}

void Sharp::Sample()
{
  Raw = analogRead(Pin);

  Middelen = Middelen - Middelen / 8 + Raw; 

  int Gemiddelde = Middelen / 8;

  Afstand = (40*148) / Gemiddelde;
}



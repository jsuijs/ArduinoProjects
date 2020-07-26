
#include "my.h" // struct definities 
                //includen *voor* alle andere code

Sharp SharpL, SharpM, SharpR;

void setup()
{
  SharpL.Pin = A0;
  SharpM.Pin = A1;
  SharpR.Pin = A3;
}

void loop()
{
  SharpSample(SharpL);
  SharpSample(SharpM);
  SharpSample(SharpR);
}


int SharpSample(Sharp &Sh)
{
  Sh.Raw = analogRead(Sh.Pin);

  Sh.Middelen = Sh.Middelen - Sh.Middelen / 8 + Sh.Raw; 

  int Gemiddelde = Sh.Middelen / 8;

  Sh.Afstand = (40*148) / Gemiddelde;
  return Sh.Afstand;
}

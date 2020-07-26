int SharpAfstand(int Pin, int &Middelen)
{
  int SensorValue = analogRead(Pin);

  Middelen = Middelen - Middelen / 8 + SensorValue; 

  int Gemiddelde = Middelen / 8;

  int Afstand = (40*148) / Gemiddelde;
  return Afstand;
}

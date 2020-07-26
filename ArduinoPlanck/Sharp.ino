// Sharp.ino



int SharpTakt(int Pin, long &Gemiddelde, long Calibratie)
{
  int SensorValue = analogRead(Pin);
  Gemiddelde = Gemiddelde - Gemiddelde / 8 + SensorValue;
  
  return Calibratie / (Gemiddelde / 8);
                                
} 
                                
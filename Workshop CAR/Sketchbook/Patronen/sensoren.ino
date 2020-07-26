
void LeesSharp()
{
  // lees sensor en reken om naar cm
  Sharp = (148 * 40) / analogRead(5); 
}

void LeesEncoder()
{ 
  static bool PrevPin;

  bool ActPin = digitalRead(6);  // pin eenmalig uitlezen
  if (PrevPin != ActPin) {                 // pin gewijzigd?
    PrevPin = ActPin;                      // waarde onthouden
    Encoder++;                             // tel op beide flanken
  } 
}


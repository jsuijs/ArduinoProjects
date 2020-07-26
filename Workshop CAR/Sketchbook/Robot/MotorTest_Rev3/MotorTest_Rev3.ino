// MotorTest_Rev3.ino - Motor Test Programma voor Rev3 motor shield

// Constantes
const int KNOP      = 2;        // knop op D2
const int LED       = 5;        // led op D13
const int MOTOR_PWM = 200;

// to support printf 
int my_putc(char c, FILE *t) {  
  return Serial.write(c);
}

//-----------------------------------------------------------------------------
// setup - wordt eenmalig uitgevoerd
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setup() {                
  // start serial
  Serial.begin(115200); 

  fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()

  pinMode(KNOP,  INPUT_PULLUP);  // knop input & pull-up
  pinMode(LED,  OUTPUT);         // led pin output

  SetupMotors();

  printf("Motortest gereed.\n");  
}

//-----------------------------------------------------------------------------
// loop - wordt continue aangeroepen na eenmalige aanroep van setup()
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void loop() 
{
  int ms;

  static int NextTaktInterval;

  ms = millis();
  if ((ms - NextTaktInterval) > 0) {
    NextTaktInterval = ms + 10;  // zet tijd voor volgende TAKT interval 

    MotorTestTakt();
  }
}

//-----------------------------------------------------------------------------
// KnopFlank - scan knoop, geef true terug bij indrukken / op neergaande flank.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool KnopFlank()
{ 
  static bool VorigeKnop;
  bool Flank = false;

  bool Knop = digitalRead(KNOP);
  if (Knop != VorigeKnop) {
    // flank (knop ingedrukt of losgelaten)
    VorigeKnop = Knop;  
    if (Knop == 0) {
      // knop is nu ingedrukt
      Flank = true;
    }
  }
  return Flank;
}

//-----------------------------------------------------------------------------
// FlashLed - Knipper led met opgegeven duty cycle
//-----------------------------------------------------------------------------
// Knipperfrequentie afhankelijk van aanroep-frequentie. 
// Bij takt van 10ms is frequentie 1 Hz
//-----------------------------------------------------------------------------
void FlashLed(char Procent)
{  
  static unsigned char Teller;

  Teller++;
  if (Teller > 100) Teller = 0;

  digitalWrite(LED, Teller < Procent);
}


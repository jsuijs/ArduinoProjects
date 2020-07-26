int LED = 13;
#define KNOP 2

// attachInterrupt example, werkt met standaard Arduino (geen library nodig)

// to support printf 
int my_putc(char c, FILE *t) {  
  if (c == '\n') Serial.write('\r');  
  return Serial.write(c);
}

void setup()
{ 
  // start serial
  Serial.begin(115200); 

  fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()
  
  pinMode(KNOP,  INPUT_PULLUP);  // knop input & pull-up
  pinMode(LED, OUTPUT);
  attachInterrupt(0, Toggle, FALLING);
}

volatile bool state = LOW;

void loop()
{
  digitalWrite(LED, state);
}

void Toggle()
{
  state = !state;
  Serial.print('*');
}



int Sharp;    // sharp meting - afstand in cm
int Encoder;  // encoder waarde (stappen)

int SavePwm;  // tijdelijk
int MotorCurve = 0;

void setup(){}

void loop() {
  static int NextTakt;

  LeesSharp();
  LeesEncoder();


  int ms = millis();
  if ((ms - NextTakt) > 0) {
    NextTakt = ms + 10;  // zet tijd voor volgende interval 

    // hier de periodieke acties voor deze interval
    //    HoofdStateMachineTakt();
    LeesSharp();

  } 
}






// to support printf 
int my_putc(char c, FILE *t) {  
  return Serial.write(c);
}

// the setup routine runs once when you press reset:
void setup() {          

  // start serial
  Serial.begin(115200); 

  fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()
  pinMode(11,  INPUT_PULLUP);  // knop input & pull-up
  printf("Opstarten gereed.\n");  
}

int Toestand = 0;
int Pwm, Duur;

void loop() {
  delay(10);

  printf("T %d, P %d, D %d\n", Toestand, Pwm, Duur);  

  switch(Toestand) {
  case 0 : {  // led uit
      // State actie
      analogWrite(6, 0);

      // Conditie
      if (digitalRead(2) == 0) { // knop ingedrukt?
        Pwm = 0;
        Toestand = 1;      
      }
      break;
    }
  case 1 : {  // led wordt feller
      // State actie
      Pwm = Pwm + 2;      
      analogWrite(6, Pwm);

      // Conditie
      if (Pwm > 250) { // (bijna) volle sterkte?
        Duur = 0;
        Toestand = 2;      
      }
      break;
    }
  case 2 : { // led 100 ticks op volle sterkte
      // State actie
      Duur++;      // Duur = Duur + 1;

      // Conditie
      if (Duur > 100) { // tijd voorbij?
        Toestand = 3;      
      }
      break;
    }
  case 3 : {  // led wordt minder fel
      // State actie
      Pwm = Pwm - 1;      
      analogWrite(6, Pwm);

      // Conditie
      if (Pwm == 0) { // led uit / pwm 0?
        Toestand = 0;      
      }
      break;
    }
  default : 
    {
      Serial.println("Ongeldige state");
      Toestand = 0;
      break;
    }
  } // einde van switch
}



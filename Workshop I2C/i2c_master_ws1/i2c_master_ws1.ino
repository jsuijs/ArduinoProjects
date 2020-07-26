#include <Wire.h>

// I2C Slave adressen van de workshop.
#define PCF8574A_I2C_ADDRESS 0x38
#define EEPROM_I2C_ADDRESS   0x57
#define DS3231_I2C_ADDRESS   0x68
#define ARDUINO_I2C_ADDRESS  0x44

// Workshop specifieke defines en globale variabelen hier

void setup() {
  Serial.begin(115200);      // start serial  
  fdevopen( &my_putc, 0);    // Koppel printf uitvoer (stdout) via my_putc() aan de serial port.

  PrintTkMsg();              // print helptekst van I2C Master Toolkit

  Wire.begin();              // start I2C 

  pinMode(8, INPUT_PULLUP);  // universele input pin

  // Workshop specifieke setup hier
}

void loop() 
{ 
  // Variabelen voor algemeen gebruik (vanaf workshop dag 3)
  byte I2cTxBuffer[32];    // zend buffer
  byte I2cRxBuffer[32];    // ontvangstbuffer
  byte r;                  // return value 

  // 500 ms lus
  static int LusTakt;
  int  ms = millis();
  if ((ms - LusTakt) > 0) {
    LusTakt = ms + 500;  // zet tijd voor volgende interval 

    // Oefening z
    static bool Knipper;
    if (Knipper) {
      Wire.beginTransmission(PCF8574A_I2C_ADDRESS);
      Wire.write(0xFF-0x20);  // Alle bits hoog, behalve bit 5 
      Wire.endTransmission();          
      Knipper = false;
    } else {
      Wire.beginTransmission(PCF8574A_I2C_ADDRESS);
      Wire.write(0xFF);  
      Wire.endTransmission();          
      Knipper = true;
    }
  } 

  // Oefening y
  Wire.requestFrom(PCF8574A_I2C_ADDRESS, 1); 
  if (Wire.available() == 1) {
    pinMode(13, OUTPUT);
    digitalWrite(13, Wire.read() & 1);
  }


  char ch = CGet();
  if (ch != 0)  {
    // Voer commando uit.
    switch(ch) {
    case 'a' : 
      {
        // Oefening a
        Wire.beginTransmission(0x38);
        Wire.write(0x00);  
        Wire.endTransmission();          
        break;
      }
    case 'b' : 
      {
        // Oefening b
        Wire.beginTransmission(PCF8574A_I2C_ADDRESS);
        Wire.write(0xFF);  
        Wire.endTransmission();          
        break;
      }
    case 'c' : 
      {
        // Oefening c
        int nr = Wire.requestFrom(PCF8574A_I2C_ADDRESS, 1); // vraag 1 byte op
        if (nr == 1) {
          int data = Wire.read();
          printf("Gelezen: %d (0x%02x)\n", data, data);
        } else {
          printf("Leesfout\n");
        }
        break;
      }
    case 'd' : 
      {
        // Oefening d
        break;
      }
    case 'e' : 
      {
        // Oefening e
        break;
      }
    case 'f' : 
      {
        // Oefening f
        break;
      }
    default : 
      {
        printf("Onbekend commando '%c'\n", ch);
        return;  // return zodat we geen 'commando uitgevoerd' printen...
      }
    }
    printf("Commmando '%c' uitgevoerd\n", ch);
  } 
}

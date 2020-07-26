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
  } 

  // Oefening y

  char ch = CGet();
  if (ch != 0)  {
    // Voer commando uit.
    switch(ch) {
    case 'a' : 
      {
        // Oefening a

        // Schrijf 1 byte, lees 3 bytes
        I2cTxBuffer[0] = 0;          // Eerste te lezen register          
        r = I2cSendReceive(0x68, 1, 3, I2cTxBuffer, I2cRxBuffer); 

        if (r == true) { // succes ? 
          HexDump(I2cRxBuffer, 3);   // Dump 3 bytes
        } 
        else {
          printf("I2C fout\n");
        }
        break;
      }
    case 'b' : 
      {
        // Oefening b
        I2cTxBuffer[0] = 0;         // register nummer
        r = I2cSendReceive(DS3231_I2C_ADDRESS, 1, 3, I2cTxBuffer, I2cRxBuffer);
        if (r == true) { // succes ? 
          printf("Tijd: %02x:%02x:%02x\n", I2cRxBuffer[2], I2cRxBuffer[1], I2cRxBuffer[0]);        
        } 
        else {
          printf("I2C fout\n");
        }          
        break;
      }
    case 'c' : 
      {
        // Oefening c
        I2cTxBuffer[0] = 0x11;     // register nummer
        r = I2cSendReceive(DS3231_I2C_ADDRESS, 1, 1, I2cTxBuffer, I2cRxBuffer);
        if (r == true) { // succes ? 
          printf("Temperatuur: %d graden\n", I2cRxBuffer[0]);
        } 
        else {
          printf("I2C fout\n");
        }          
        break;
      }
    case 'd' : 
      {
        // Oefening d
        I2cTxBuffer[0] = 0x11;     // register nummer
        r = I2cSendReceive(DS3231_I2C_ADDRESS, 1, 2, I2cTxBuffer, I2cRxBuffer);
        if (r == true) { // succes ? 
          float temperatuur = I2cRxBuffer[0] + I2cRxBuffer[1] / 256.0;
          Serial.print("De temperatuur is ");
          Serial.print(temperatuur);
          Serial.print(" graden\n");
        } 
        else {
          printf("I2C fout\n");
        }          
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

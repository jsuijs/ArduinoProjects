#include <Wire.h>

// I2C Slave adressen van de workshop.
#define PCF8574A_I2C_ADDRESS 0x38
#define EEPROM_I2C_ADDRESS   0x57
#define DS3231_I2C_ADDRESS   0x68
#define ARDUINO_I2C_ADDRESS  0x44

// Workshop specifieke defines en globale variabelen hier

// tweede eeprom
#define LOG_BLOCK_SIZE         8   // Macht van 2, maximaal 32
#define LOG_MAX_DATA           7   // maximaal blocksize-1 en maximaal 29 
#define LOG_NR_BLOCKS       4096   // max eeprom grootte (bytes) / LOG_BLOCK_SIZE
#define LOG_EEPROM_ADDRESS  0x53   // I2C slave adres




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

  // vaste interval lus
  static long LusTakt;
  long  ms = millis();
  if ((ms - LusTakt) > 0) {
    LusTakt = ms + 60000L;  // zet tijd voor volgende interval 

    // Oefening z
    I2cTxBuffer[0] = 0;  // vanaf adres 0
    r = I2cSendReceive(DS3231_I2C_ADDRESS, 1, 0X13, I2cTxBuffer, I2cRxBuffer);

    byte LogBuffer[7];
    LogBuffer[0] = I2cRxBuffer[0X05];  // maand
    LogBuffer[1] = I2cRxBuffer[0X04];  // dag
    LogBuffer[2] = I2cRxBuffer[0X02];  // uur
    LogBuffer[3] = I2cRxBuffer[0X01];  // min
    LogBuffer[4] = I2cRxBuffer[0X00];  // sec
    LogBuffer[5] = I2cRxBuffer[0X11];  // temp (graden)
    LogBuffer[6] = I2cRxBuffer[0X12];  // temp (remainder)
    LogWrite(LogBuffer);

  } 

  // Oefening y

  char ch = CGet();
  if (ch != 0)  {
    // Voer commando uit.
    switch(ch) {
    case 'a' : 
      {
        // Oefening a
        LogReadOpen();  // lees vanaf 1e record
        byte Data[7];
           
        for (;;) {
          bool r = LogRead(Data);

          if (r) {
            // success
            // print datum/tijd (jaar wordt niet gelogd en verzinnen we hier)
            printf("2017-%02x-%02x %02x:%02x:%02x ", 
              Data[0], Data[1], Data[2], Data[3], Data[4]);
                
            float temperatuur = (Data[5] * 256 + Data[6]) / 256.0;
            Serial.println(temperatuur);
          } 
          else {
            printf("Log end.\n");
            return;
          }
        }        
        break;
      }
    case 'b' : 
      {
        // Oefening b
        break;
      }
    case 'c' : 
      {
        // Oefening c
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


#include <Wire.h>

// I2C Slave adressen van de workshop.
#define PCF8574A_I2C_ADDRESS 0x38
#define EEPROM_I2C_ADDRESS   0x57
#define DS3231_I2C_ADDRESS   0x68
#define ARDUINO_I2C_ADDRESS  0x44

// Workshop specifieke defines en globale variabelen hier
int Reg5 = 0;
#define SLAVE_ADDRESS 0x88/2

void setup() {
  Serial.begin(115200);      // start serial
  fdevopen( &my_putc, 0);    // Koppel printf uitvoer (stdout) via my_putc() aan de serial port.

  PrintTkMsg();              // print helptekst van I2C Master Toolkit

  Wire.begin();              // start I2C

  pinMode(8, INPUT_PULLUP);  // universele input pin

  // Workshop specifieke setup hier

  // initialiseer slave registers
  pinMode(13, OUTPUT);
  SchrijfOnzeSlave(4, 0);  // zet slave register 4 op 0
  SchrijfOnzeSlave(5, Reg5);  // Schrijf waarde van var Reg5 naar slave register 5
}

void SchrijfOnzeSlave(byte Register, byte Waarde)
{
  byte I2cTxBuffer[2];

  I2cTxBuffer[0] = Register;
  I2cTxBuffer[1] = Waarde;
  I2cSendReceive(SLAVE_ADDRESS, 2, 0, I2cTxBuffer, NULL);
}

void loop()
{
  // Variabelen voor algemeen gebruik (vanaf workshop dag 3)
  byte I2cTxBuffer[32];    // zend buffer
  byte I2cRxBuffer[32];    // ontvangstbuffer
  byte r;                  // return value

  // 100 ms lus
  static int LusTakt;
  int  ms = millis();
  if ((ms - LusTakt) > 0) {
    LusTakt = ms + 100;  // zet tijd voor volgende interval

    // Oefening z
    if (!AddressProbe(SLAVE_ADDRESS)) {
      printf("Arduino slave op adres 0x%02x reageert niet\n", SLAVE_ADDRESS);
      LusTakt += 900;  // extra wachttijd
      return;
    }
    // Lees register 1 en stuur LED aan.
    I2cTxBuffer[0] = 1;  // te lezen register
    I2cSendReceive(SLAVE_ADDRESS, 1, 1, I2cTxBuffer, I2cRxBuffer);  // schrijf register nummer en lees byte / register
    digitalWrite(13, !I2cRxBuffer[0]);  // Led aan of uit, afhankelijk van gelezen waarde.

    // Lees register 2+3 en print 16 bit waarde
    // (deze register gaan we vullen met 10 bits van de analoge ingang)
    I2cTxBuffer[0] = 2;  // te lezen register (vanaf)
    I2cSendReceive(SLAVE_ADDRESS, 1, 2, I2cTxBuffer, I2cRxBuffer);  // scrhijf register nummer en lees 2 bytes / registers
    int Waarde = (int)I2cRxBuffer[0] * 256 + I2cRxBuffer[1];  // voeg 2 bytes samen tot 1 integer
    printf("Waarde analoge ingang: %d\n", Waarde);
  }

  // Oefening y
  char ch2 = MatrixKeyScan();
  switch (ch2) {
  case '1':
    {
      SchrijfOnzeSlave(4, 1);  // zet slave register 4 op 1
      break;
    }
  case '2':
    {
      SchrijfOnzeSlave(4, 0);  // zet slave register 4 op 0
      break;
    }
  case '4':
    {
      Reg5 /= 2;
      if (Reg5 < 0) Reg5 = 0;
      SchrijfOnzeSlave(5, Reg5);  // Schrijf waarde van var Reg5 naar slave register 5
      break;
    }
  case '5':
    {
      Reg5 = 1 + Reg5 * 2;
      if (Reg5 > 255) Reg5 = 255;
      SchrijfOnzeSlave(5, Reg5);  // Schrijf waarde van var Reg5 naar slave register 5
      break;
    }
  case '7':
    {
      SchrijfOnzeSlave(0, 1);  // Geef commando 1
      break;
    }
  case '8':
    {
      SchrijfOnzeSlave(0, 2);  // Geef commando 2
      break;
    }
  case '*':
    {
      SchrijfOnzeSlave(0, 3);  // Geef commando 3
      break;
    }
  case '0':
    {
      SchrijfOnzeSlave(0, 4);  // Geef commando 4
      break;
    }
  case -1:
    {
      break;
    }
  default:
    {
      // ongebruikte toets
      printf("Toets '%c'\n", ch2);
      break;
    }
  }
  if (ch2 != -1) printf("Key %c\n", ch2);

  char ch = CGet();
  if (ch != 0)  {
    // Voer commando uit.
    switch(ch) {
    case 'a' :
      {
        // Oefening a
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
        printf("Onbekend cmd '%c'\n", ch);
        return;  // return zodat we geen 'commando uitgevoerd' printen...
      }
    }
    printf("Cmd '%c' uitgevoerd\n", ch);
  }
}

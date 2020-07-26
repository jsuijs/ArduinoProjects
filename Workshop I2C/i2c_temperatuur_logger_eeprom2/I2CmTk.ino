//-----------------------------------------------------------------------------
// I2CmTk.ino - I2C Master Toolkit (c) 2016-2017 Karel Dupain & Joep Suijs
//-----------------------------------------------------------------------------
// Deze toolkit is ontwikkeld voor de Workshop 'Arduino & I2C'.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//-----------------------------------------------------------------------------


//#define NO_DEMO_COMMANDS // Disable Demo commando's van de toolkit. Nuttig als de Toolkit
// wordt gebruikt voor een ander project dan de workshop.


int I2cDebug = 0;

/*=====================================================================
 TCommand :
 ---------------------------------------------------------------------*/

class TCommand
{
  int  State     ;
  int  Position  ;

  bool HexMode   ;
  bool Negative  ;

  int  NrIndex   ;
  int  LastError ; // for Execute &() MatchCommand() only

  bool Add( int Value )
  {
    if( Position >= ( (int) sizeof( Cmd ) - 1 ) ) return false ;

    Cmd[ Position++ ] = (unsigned char ) Value ;
    Cmd[ Position   ] = 0     ;

    return true ;
  }

public:
  int  Number[3] ;
  int  ParamCount;

  char Cmd[ 32 ] ;

  TCommand()
  {
    Clear() ;
  }

  // Clear - Signal we want to restart
  void Clear()
  {
    State      = 0     ;
  }

  int GetLine( int Value )  // 0 doe niets  1 ok    < 0 error
  {
    if( Value < 0 ) return 0  ;
    if( Value == 0x0a) Value = 0x0d;

    //printf("GL Value: %d (%c), State: %d, NrIndex: %d, ParamCount: %d\n", Value, Value, State, NrIndex, ParamCount);

    switch( State )
    {
    case 0:  // wait for first alpha-char 
      {
        if( isspace( Value ) ) return 0 ;

        if (Value == '`') 
        {
          // repeat previous command
          State = 500;  // process whitespaces until line end
          return 0;
        } 
        else 
        {
          // New input -> clear internal data.
          Position   = 0     ;
          NrIndex    = 0     ;
          HexMode    = false ;
          Negative   = false ;
          ParamCount = 0     ;

          Number[0]  = 0     ;
          Number[1]  = 0     ;
          Number[2]  = 0     ;
        }

        if ( Value > ' ' )
        {
          Add( Value ) ;
          State = 100 ;
          return 0 ;
        }
        State = 900;
        return -1 ;
      }
    case 100:  // wait for other command word chars
      {
        if( isalnum( Value ) == false )
        {
          if( isspace( Value ))
          {
            if( Value == 0x0d ) break ;

            State = 200 ;
            return 0 ;
          }
          State = 900;
          return -1 ;
        }
        Add( Value ) ;

        return 0 ;
      }
    case 200:   // First character of parameter/number
      {
        if( Value == 0x0d ) break ;
        if( isspace( Value )) return 0 ;

        if( Value == '-' ) 
        { 
          Negative = true ; 
          Number[ NrIndex ] = 0 ;
          HexMode = false;
          State = 400 ;
          return 0 ;
        }
        if( isdigit( Value ) == false ) return -2 ;

        if( Value == '0' )
        {
          State = 300 ; 
          return 0 ; // '0' has to be followed by 'x' for
          // hex number or to be stand-alone 
        }

        Number[ NrIndex ] = Value - 0x30 ;
        HexMode = false;
        State = 400 ;
        return 0 ;
      }
    case 300:  // wait for 'x' ('0x', hex pre-amble)
      {
        ParamCount = NrIndex + 1;

        if( ( Value | 0x20 ) == 'x' ) { 
          HexMode = true ; 
          State = 400 ; 
          return 0 ; 
        }

        if( Value == 0x0d ) break ;

        if ( isspace( Value ) )
        {
          NrIndex++ ;

          if( NrIndex > 2 ) State = 500 ;
          else State = 200 ;

          return 0 ;
        }
        if( isdigit( Value ) ) 
        {
          // '0' followed by digit -> decimal input
          Number[ NrIndex ] = Value - 0x30 ;
          HexMode = false;
          State = 400 ;
          return 0;
        }

        State = 900;
        return -3 ;  // no 'x', number, whitespace or line-end -> bad sequence
      }
    case 400: // collect number (hex or decimal)
      {

        ParamCount = NrIndex + 1;

        if( Value == 0x0d ) break ;

        if ( isspace( Value ) )
        {
          NrIndex++ ;

          if( NrIndex > 2 ) State = 500 ;  // last parameter (allowed)
          else 
          {
            Negative = false;
            State = 200 ;  // scan (optional) next parameter
          }

          return 0 ;
        }

        if( HexMode )
        {
          if( isxdigit( Value ) == false ) return -5 ;

          if( isalpha( Value ) ) Value = ( Value & 0xdf ) - 55 ;
          else Value = ( Value - 0x30 ) ;

          Number[ NrIndex ] = Number[ NrIndex ] * 16 + Value ;

        } 
        else
        {
          if( isdigit( Value )  == false ) return -6 ;

          Value = Value - 0x30 ;
          if( Negative) Value *= -1;

          Number[ NrIndex ] = Number[ NrIndex ] * 10 + Value ;
        }

        return 0 ;
      }
    case 500:  // allow only whitespaces / line-end
      {
        if( Value == 0x0d ) break ;

        if( isspace( Value ) ) return 0 ;

        State = 900;
        return -7 ;
      }
    case 900:  // wait for line-end (error recovery)
      {
        if( Value == 0x0d ) break ;

        return 0 ;
      }
    }
    State = 999 ;

    return 1 ; // Command line ready
  }

  void Print()
  {
    printf( "Cnt = %d  A=%u  B=%u  C=%u  Cmd=%s\r\n", ParamCount, Number[0], Number[1], Number[2], Cmd) ;
  }


  // return true on success (command & parameter match)
  byte MatchCommand(const char *Keyword, byte NrParams)
  {
    if (LastError == 0)         return false;  // this command is already executed.
    if (strcmp(Cmd, Keyword))   return false;  // not this command.
    LastError = 1;  // command recognised, but (maybe) incorrect nr of params
    if (ParamCount != NrParams) return false;  // incorrect nr of params
    LastError = 0;  // success
    return true;    // execute command
  }

  // return:
  // 0 = no command
  // 1 = internal command executed
  // 2 = internal command error (incorrect nr of params, command error)
  // >2 = single command character
  char Execute()
  {  
    LastError = 2;  // unknown command

    if (MatchCommand("?",         0)) PrintTkMsg();                                      
    if (MatchCommand("debug",     1)) I2cDebug = Number[0];                              
    if (MatchCommand("scan",      0)) BusScan();                                         
    if (MatchCommand("ram",       0)) printf("Ram: %d bytes vrij\n", FreeRam());         
    if (MatchCommand("fill",      3)) EepromFill(Number[0], Number[1], Number[2]);       
    if (MatchCommand("logdump",   0)) LogDump();                                         
    if (MatchCommand("logclear",  0)) LogClear();                                        
#ifndef NO_DEMO_COMMANDS                                                            
    if (MatchCommand("logwrite",  0)) DemoLogWrite();                                    
    if (MatchCommand("logread",   0)) DemoLogRead();                                     
    if (MatchCommand("epatroon",  0)) DemoEepromPattern();                               
    if (MatchCommand("edump",     0)) DemoEepromDump();                                  
    if (MatchCommand("tijd",      0)) DemoPrintTijd();                                   
    if (MatchCommand("tijd",      3)) DemoSetTijd(Number[0], Number[1], Number[2]);      
    if (MatchCommand("blink",     0)) DemoBlink();                       
    if (MatchCommand("running",   1)) DemoRunningLight(Number[0]);                       
#endif
    if (MatchCommand("rnb",       1)) I2cReader(Number[0], 0, 1,       0);               
    if (MatchCommand("rnw",       1)) I2cReader(Number[0], 0, 2,       0);               
    if (MatchCommand("rbb",       2)) I2cReader(Number[0], 1, 1, Number[1]);             
    if (MatchCommand("rbw",       2)) I2cReader(Number[0], 1, 2, Number[1]);             
    if (MatchCommand("rwb",       2)) I2cReader(Number[0], 2, 1, Number[1]);             
    if (MatchCommand("rww",       2)) I2cReader(Number[0], 2, 2, Number[1]);             
    if (MatchCommand("wnb",       2)) I2cWriter(Number[0], 0, 1,       0, Number[1]);    
    if (MatchCommand("wnw",       2)) I2cWriter(Number[0], 0, 2,       0, Number[1]);    
    if (MatchCommand("wbb",       3)) I2cWriter(Number[0], 1, 1, Number[1], Number[2]);  
    if (MatchCommand("wbw",       3)) I2cWriter(Number[0], 1, 2, Number[1], Number[2]);  
    if (MatchCommand("wwb",       3)) I2cWriter(Number[0], 2, 1, Number[1], Number[2]);  
    if (MatchCommand("www",       3)) I2cWriter(Number[0], 2, 2, Number[1], Number[2]);  
    if (MatchCommand("rn",        2)) I2cReader2(Number[0], 0, Number[1],       0);      
    if (MatchCommand("rb",        3)) I2cReader2(Number[0], 1, Number[2], Number[1]);    
    if (MatchCommand("rw",        3)) I2cReader2(Number[0], 2, Number[2], Number[1]);    

    switch(LastError) {
    case 2 : 
      {  // unknown command
        if (Cmd[1] == 0) {
          // single char keyword => return char
          return Cmd[0];
        } 
        else {
          printf("Onbekend cmd '%s' (2)\n", Cmd);
          return 2;
        }
        break;
      }
    case 1 : 
      {
        printf("Error: # params - cmd '%s'\n", Cmd);
        return 2;
      }
    case 0 : 
      {
        printf("TkCmd '%s' gereed.\n", Cmd);
        return 1;
      }
    default : 
      {
        printf("Error 0924\n");
        return 2;
      }
    }   
  }
} 
;


/*==============================================
 Variables :
 ----------------------------------------------*/

String       str           ;

TCommand Command;

//-----------------------------------------------------------------------------
// CGet - Get single char command from serial & drive Toolkit command line
//-----------------------------------------------------------------------------
// The single char commands are intended to support the workshop excercises.
// Using this function automatically enables the I2CmTK core and demo commands.
//-----------------------------------------------------------------------------
char CGet()
{  
  static bool First = true;

  if (First) {
    First = false;
    Serial.setTimeout(50);           
  }

  if( Serial.available() > 0)
  {
    int r = Command.GetLine(Serial.read());
    if (r == 0) return 0;  // still reading line

    if (r < 0) {
      printf("Cmd parse err %d\n", r);
      Command.Clear();
      return 0;
    }

    //Command.Print() ;

    char ch = Command.Execute();
    Command.Clear();

    if (ch > 2) return ch;
  } 
  return 0;
}

const byte KeyCode[] = "*7410852";

//----------------------------------------------------------------------------- 
// MatrixKeyScan
//----------------------------------------------------------------------------- 
// return: key char (ascii, one shot) or -1 when no key pressed
//----------------------------------------------------------------------------- 
int MatrixKeyScan()
{  
  static char LastKey = -2; // -2 = setup, -1 = no key, 99 = key reported, other = real key

  if (LastKey == -2) {
    // Setup
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(7, INPUT_PULLUP);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
  }

  int Pins = ((PIND>>4) & 0xF) ^ 0xf;
  if (Pins == 0) {
    // no button pushed
    LastKey = -1;
    return -1;
  }

  // Which button (row) is pressed?  
  pinMode(2, INPUT);
  delay(1);
  int Pins2 = ((PIND>>4) & 0xF) ^ 0xf;
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  int NewKey = -1; 
  if (Pins2 == 0) NewKey += 4;

  if (Pins & 0x1) {
    NewKey += 1;
  } 
  else if (Pins & 0x2) {
    NewKey += 2;
  } 
  else if (Pins & 0x4) {
    NewKey += 3;
  } 
  else if (Pins & 0x8) {
    NewKey += 4;
  }

  //printf("Pins: %02x, Pins2: %02x, NewKey: %d, LastKey: %d\n", Pins, Pins2, NewKey, LastKey);

  delay(50);  // debounce  
  if (LastKey == -1) {
    LastKey = NewKey;
    return -1;
  }
  if (LastKey == 99) {
    return -1;
  }
  if (LastKey == NewKey) {
    LastKey = 99;
    return KeyCode[NewKey];
  }
  return -1;
}

//----------------------------------------------------------------------------- 
// PrintTkMsg - print I2C Master Toolkit message
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void PrintTkMsg()
{
  Serial.print(F("\nI2CmTk - I2C Master Toolkit versie 0.82\n"));
  Serial.print(F("Ontwikkeld voor de Workshop 'Arduino & I2C'.\n"));
  Serial.print(F("(c) 2016-2017 Karel Dupain & Joep Suijs\n"));
  printf("Gecompileerd: %s %s\n", __DATE__, __TIME__ ); 
}

//-----------------------------------------------------------------------------
// my_putc - links stdout (printf) to serial
//-----------------------------------------------------------------------------
// put 'fdevopen( &my_putc, 0);' in setup();
//-----------------------------------------------------------------------------
int my_putc(char c, FILE *t) {  
  if (c == '\n') Serial.write('\r');
  return Serial.write(c);
}

//-----------------------------------------------------------------------------
// FreeRam - actueel vrij geheugen in bytes.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int FreeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

//-----------------------------------------------------------------------------
// AddressProbe - check if slave is present.
//-----------------------------------------------------------------------------
// Return: true when slave is present
//-----------------------------------------------------------------------------
bool AddressProbe(int I2cSlaveAddress)
{
  Wire.requestFrom(I2cSlaveAddress, 1);    // request 2 bytes from slave device #112

  if (Wire.available() != 0) { 
    Wire.read(); 
    return true;  // slave present
  }
  return false;  // not present
}

//-----------------------------------------------------------------------------
// BusScan - Scan i2c bus for slaves present.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BusScan()
{
  for (int I2cSlaveAddress = 0; I2cSlaveAddress<127; I2cSlaveAddress++) {

    if (AddressProbe(I2cSlaveAddress)) {
      printf("%2x", I2cSlaveAddress);
    }
    printf(".");
    if ((I2cSlaveAddress & 0x01F) == 0x1F) printf("\n");
  }
  printf("\n");
}

//----------------------------------------------------------------------------- 
// I2cSendReceive - Send and/or Receive data to/from i2c slave.
//-----------------------------------------------------------------------------
// return: true on success
//-----------------------------------------------------------------------------
bool I2cSendReceive(byte I2cSlaveAddress, byte TxCount, byte RxCount, const byte *TxBuffer, byte *RxBuffer)
{ 
  byte r;

  if (I2cDebug) {
    printf("I2cSendReceive(%d %d %d)\n", I2cSlaveAddress, TxCount, RxCount);
  }

  if (TxCount > 0) {
    if (I2cDebug > 1) {
      printf("TxBuf:\n");
      HexDump(TxBuffer, TxCount);
    }

    Wire.beginTransmission(I2cSlaveAddress);
    for (int i=0; i<TxCount; i++) { 
      Wire.write(TxBuffer[i]); // 
    }
    r = Wire.endTransmission();  
    if (r != 0) return false;  // error
  }

  if (RxCount > 0) {

    for (int i=0; i<RxCount; i++) RxBuffer[i] = 0; 

    Wire.requestFrom(I2cSlaveAddress, RxCount);    // request byte(s) from slave device

    if (Wire.available() != RxCount) return false;  // error

    for (int i=0; i<RxCount; i++) { 
      RxBuffer[i] = Wire.read(); 
    }
    if (I2cDebug > 1) {
      printf("RxBuf:\n");
      HexDump(RxBuffer, RxCount);
    }
  }
  return true;  // success
}

//-----------------------------------------------------------------------------
// HexDump - Dump Data in hex format
//-----------------------------------------------------------------------------
// No offset, so address displayed at the start of each line starts at 0.
//-----------------------------------------------------------------------------
void HexDump(const void *Data, int Length)
{
  HexDump(Data, Length, 0);
}

/*=====================================================================
 HexDump :
 ---------------------------------------------------------------------*/
// Parameters: 
//    Data   - data to be dumped
//    Length - nr of bytes to be dumped
//    Offset - offset of address (from 0), displayed at the start of each line.
//-----------------------------------------------------------------------------
void HexDump( const void *Data, unsigned int Length, unsigned int Offset)
{
  unsigned char *data    = (unsigned char *)Data    ;

  unsigned int Track1 = 0 ;
  unsigned int Track2 = 0 ;

  for( unsigned int Index=0 ; Index < Length ; Index = Index+16 )
  {
    printf( "%04x: ", Offset + Index ) ;

    for( unsigned int j=0 ; j < 16 ; j++ )
    {
      if( Track1 < Length ) printf( "%02x", data[ Index+j ] ) ;
      else printf( "  " ) ;

      printf( " " ) ;

      Track1++ ;
    }

    printf( " "  ) ;

    for( unsigned int j=0 ; j < 16 ; j++ )
    {
      if( Track2 < Length )
      {
        if( data[ Index+j ] < 32 ) printf( "." ) ;
        else
        {
          if( data[ Index+j ] < 127 ) printf( "%c", data[ Index+j ] ) ;
          else printf( "."                   ) ;
        }
      } 
      else printf( " " ) ;

      Track2++ ;
    }

    printf( "\r\n" ) ;
  }
}

//-----------------------------------------------------------------------------
// _SetupRegisterAddress - support function
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void _SetupRegisterAddress(byte *TxBuffer, int RegBytes, int RegAddr)
{
  // Setup Address
  switch(RegBytes) {
  case 0 : 
    {
      // no address
      break;
    }
  case 1 : 
    {
      // byte address
      TxBuffer[0] = RegAddr;
      break;
    }   
  case 2 : 
    {
      // word address
      TxBuffer[0] = RegAddr >> 8;
      TxBuffer[1] = RegAddr;
      break;  
    }
  default : 
    {
      I2cError(0,947);
      break;
    }
  }
}

//-----------------------------------------------------------------------------
// I2cReader - support function for READ commands
//-----------------------------------------------------------------------------
// RegBytes:  nr of bytes, 0, 1 or 2, in RegAddr
// DataBytes: nr of bytes, 1 or 2, to be read
//-----------------------------------------------------------------------------
static void I2cReader(int Slave, int RegBytes, int DataBytes, int RegAddr)
{  
  unsigned int Data;
  bool r;
  byte TxBuffer[2], RxBuffer[2];

  _SetupRegisterAddress(TxBuffer, RegBytes, RegAddr);
  r = I2cSendReceive(Slave, RegBytes, DataBytes, TxBuffer, RxBuffer);

  if (!r) {
    I2cError(Slave, 1210);
    return;
  }

  // process data. 
  switch(DataBytes) {
  case 1 : 
    {
      // byte data
      Data = RxBuffer[0];
      break;
    }   
  case 2 : 
    {
      // word data
      Data = RxBuffer[0];
      Data *= 256;
      Data += RxBuffer[1];
      break;  
    }
  default : 
    {
      I2cError(Slave, 927);
      return;
    }
  }

  printf("I2c read 0x%02x, ", Slave);
  if (RegBytes != 0) {   
    printf("Reg: %d (0x%02x), Data: %u (0x%x)\n", RegAddr, RegAddr, Data, Data);
  } 
  else {
    printf("Data: %u\n", Data);
  }    
}

//-----------------------------------------------------------------------------
// I2cWriter - support function for WRITE commands
//-----------------------------------------------------------------------------
// RegBytes:  nr of bytes, 0, 1 or 2, in RegAddr
// DataBytes: nr of bytes, 1 or 2, to write
//-----------------------------------------------------------------------------
static void I2cWriter( int Slave, int RegBytes, int DataBytes, int RegAddr, int Data)
{  
  bool r = 1;
  byte TxBuffer[4];

  _SetupRegisterAddress(TxBuffer, RegBytes, RegAddr);

  // Setup data. 
  switch(DataBytes) {
  case 1 : 
    {
      // byte data
      TxBuffer[RegBytes+0] = Data;
      break;
    }   
  case 2 : 
    {
      // word data
      TxBuffer[RegBytes+0] = Data/256;
      TxBuffer[RegBytes+1] = Data;
      break;  
    }
  default : 
    {
      I2cError(Slave, 929);
      break;
    }
  }

  r = I2cSendReceive(Slave, RegBytes+DataBytes, 0, TxBuffer, NULL);

  if (!r) {
    I2cError(Slave, 1214);
    return;
  }

  printf("Slave: 0x%02x, ", Slave);
  if (RegBytes != 0) {
    printf("register %d (0x%02x) set to %d (0x%02x)\n", RegAddr, RegAddr, Data, Data);
  } 
  else {
    printf("Data: %d (0x%02x)\n", Data, Data);      
  }
}

//-----------------------------------------------------------------------------
// I2cReader2 - support function for READ commands - Read & dump arbitrary nr of bytes
//-----------------------------------------------------------------------------
// RegBytes:  nr of bytes, 0, 1 or 2, in RegAddr
// DataBytes: nr of bytes, (1..32)
//-----------------------------------------------------------------------------
static void I2cReader2(int Slave, int RegBytes, int DataBytes, int RegAddr)
{  
  byte TxBuffer[2];
  byte RxBuffer[34];

  if (DataBytes > 32) {
    printf("#bytes limited to 32!\n");
    DataBytes = 32;         
  }

  _SetupRegisterAddress(TxBuffer, RegBytes, RegAddr);

  if (!I2cSendReceive(Slave, RegBytes, DataBytes, TxBuffer, RxBuffer)) {     
    printf("I2cRead() - comms err\n"); 
    return;
  }
  HexDump(RxBuffer, DataBytes);   
}             

//-----------------------------------------------------------------------------
// EepromFill - Fill in 16 byte chuncks
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EepromFill(int Slave, unsigned int Size, int Value)
{ 
  byte TxBuffer[34];
  bool r;

  delay(10);  // allow pending write cycle to finish

  // fill txbuffer with data
  for (int i=2; i<18; i++) {
    TxBuffer[i] = Value;
  }

  for (unsigned int i=0; i<Size; i+= 16) {
    TxBuffer[0] = i >> 8;
    TxBuffer[1] = i & 0xFF;
    r = I2cSendReceive(Slave, 18, 0, TxBuffer, NULL);
    if (!r) {
      printf("\nEeFill err %d.\n", i);
      return;
    }

    if ((i & 0x3F) == 0)  printf(".");  // print dot in one of 64 runs.

    while (AddressProbe(Slave) == false) { 
      // wait for eeprom to finish write
    }
  }
  printf("\nEeFill done\n");
}

//-----------------------------------------------------------------------------
// Bcd - convert decimal value to bcd.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
byte Bcd(byte Decimal)
{
  return (Decimal / 10) * 16 + (Decimal % 10);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void I2cError(int Slave, int ErrorNr)
{
  printf("I2c error bij communicatie met slave 0x%02x (%d).\n", Slave, ErrorNr);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Eeprom log routines.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Algemeen:
// - Het eerste byte van een actief log record is 0. 
// - LogClear schrijft 255 naar alle locaties van de eeprom.
// - De maximale grootte van een log record is 29 bytes (netto data, ex dat 1e byte)
//   (Dit is een Arduino beperking. Er is een work-around is mogelijk, maar
//    heeft performance impact of vereist interrupts).                              
// - De maximale grootte van een log record is (in deze opzet) ook beperkt tot
//   de block-grootte van de eeprom. Dit is in de praktijk geen beperking omdat
//   deze voor de 24lc32 al 32 bytes groot is en voor grotere eeproms wordt deze groter.
// - LOG_BLOCK_SIZE moet een macht van 2 zijn, zodat een record altijd binnen een 
//   block van de eeprom past.
// - LogDataSize is de werkelijke log grootte van een log record
//-----------------------------------------------------------------------------

// setup defaults:
#ifndef LOG_BLOCK_SIZE
#define LOG_BLOCK_SIZE 16        // Macht van 2, maximaal 32 (zie boven)
#endif
#ifndef LOG_MAX_DATA
#define LOG_MAX_DATA   15        // maximaal blocksize-1 en maximaal 29 
#endif
#ifndef LOG_NR_BLOCKS
#define LOG_NR_BLOCKS 256        // max eeprom grootte (bytes) / LOG_BLOCK_SIZE
#endif
#ifndef LOG_EEPROM_ADDRESS
#define LOG_EEPROM_ADDRESS 0x57  // I2C slave adres
#endif


// Interne variabelen:
int  LogWriteRecord;   // next record (block) to write
int  LogReadRecord;    // next record (block) to read
bool LogFirst = true;  // First call (or not yet initialzed)

//-----------------------------------------------------------------------------
// LogDump - Dump data in logfile.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void LogDump()
{  
  byte Buffer[32];

  printf("LWR: %d\n", LogWriteRecord); 

  LogReadRecord = 0;  // LogReadFirst equivalent
  for (bool r=true;r;) {
    r = LogRead(Buffer);
    printf("(%02d %d) ", LogReadRecord, r);
    HexDump(Buffer, LOG_MAX_DATA + 1);    
  } 
}


//-----------------------------------------------------------------------------
// LogInit - Internal function - do not call directly
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void LogInit()
{
  if (LogFirst) {
    if (AddressProbe(LOG_EEPROM_ADDRESS)) {
      // eeprom present.
      LogFirst = false;
    } 
    else {   
      printf("Logging not ready... (0x%x)\n", LOG_EEPROM_ADDRESS); 
    }
    return;
  }

  // wait in case the eeprom is busy...
  while (!AddressProbe(LOG_EEPROM_ADDRESS)) {
  }
}

//-----------------------------------------------------------------------------
// LogClear - wis de eeprom en start nieuwe log-reeks
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void LogClear()
{
  LogInit();  // starup check

  // fill eeprom with 255
  EepromFill(LOG_EEPROM_ADDRESS, LOG_BLOCK_SIZE * LOG_NR_BLOCKS, 255);

  // reset 'pointer'
  LogWriteRecord = 0;
}

//-----------------------------------------------------------------------------
// LogWrite - Schrijf Data naar de logfile.
//-----------------------------------------------------------------------------
// De lengte van Data is vastgelegd in LogDataSize. Het beste is om deze
// waarde eenmalig in te stellen en niet meer te veranderen.
//-----------------------------------------------------------------------------
void LogWrite(const byte *Data)
{ 
  byte I2cTxBuffer[LOG_MAX_DATA+3];

  LogInit();  // starup check

  if (LogWriteRecord == 0) {
    // check if we need to append to previous logging
    byte Data[LOG_MAX_DATA];
    bool r;
    int Save = LogReadRecord;
    LogReadRecord = 0;    
    do { 
      r = LogRead(Data); 
    }
    while (r);
    LogWriteRecord = LogReadRecord;
    LogReadRecord = Save;
  }

  if (LogWriteRecord == LOG_NR_BLOCKS) {
    printf("Log: eeprom vol\n");
    LogWriteRecord ++;
  }
  if (LogWriteRecord >= LOG_NR_BLOCKS) return;  // eeprom full

  // Prepare Tx msg header  
  int Address = LogWriteRecord * LOG_BLOCK_SIZE;
  I2cTxBuffer[0] = Address / 256;
  I2cTxBuffer[1] = Address & 0xFF;
  I2cTxBuffer[2] = 0;  // valid record (not 255)

  // Append data
  for (int i=0;i<LOG_MAX_DATA; i++) {
    I2cTxBuffer[i+3] = Data[i];
  }
  I2cSendReceive(LOG_EEPROM_ADDRESS, LOG_MAX_DATA + 3, 0, I2cTxBuffer, NULL);

  LogWriteRecord ++;
}

//-----------------------------------------------------------------------------
// LogReadOpen - Zet pointer op eerste record uit de logfile.
//----------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------
void LogReadOpen()
{
  LogReadRecord = 0;
}

//-----------------------------------------------------------------------------
// LogRead - Lees het voglende record uit de logfile.
//----------------------------------------------------------------------------- 
// De lengte van Data is vastgelegd in LogDataSize. Het beste is om deze
// waarde eenmalig in te stellen en niet meer te veranderen.
//
// De Data buffer moet minimaal LogDataSize lang zijn.
// return: true = succces (niet einde van log)
//-----------------------------------------------------------------------------
bool LogRead(byte *Data)
{
  byte I2cTxBuffer[2];
  byte I2cRxBuffer[LOG_MAX_DATA+1];

  LogInit();  // starup check

  if (LogReadRecord >= LOG_NR_BLOCKS) return false;  // end of eeprom

  // prepare Tx message  
  int Address = LogReadRecord * LOG_BLOCK_SIZE;
  I2cTxBuffer[0] = Address / 256;
  I2cTxBuffer[1] = Address & 0xFF;

  bool r = I2cSendReceive(LOG_EEPROM_ADDRESS, 2, LOG_MAX_DATA + 1, I2cTxBuffer, I2cRxBuffer);

  if (!r) return false;  // i2c comms failed 

  // Copy data
  for (int i=0; i<LOG_MAX_DATA; i++) {
    Data[i] = I2cRxBuffer[i+1];
  }

  if (I2cRxBuffer[0] == 255) return false; // no valid data

  LogReadRecord ++;  // next record  
  return true;       // success
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DEMO funties
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Deze functies zijn specifiek ontwikkeld voor de 'Arduino en I2C' workshop.
// Ze maken bijvoorbeeld gebruik van de specifieke slaves van de workshop, 
// printen data etc. Dit in tegenstelling tot de 'I2Cm toolkit' functies, die
// generiek zijn en daardoor bruikbaar voor ieder project waar de arduino
// als I2C master wordt ingezet.
//
// De variabelen en commentaar-regels van de algemene toolkit zijn (over het 
// algemeen) in het Engels, terwijl de Demo functies zoveel mogelijk in het
// Nederlands zijn.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DemoLogWrite - Routine om (via menu) LogWrite te demonstreren
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void DemoLogWrite()
{
  byte Data[LOG_MAX_DATA];

  long Tijd = millis() /1000; // tijd in seconde stappen
  Data[0] = Tijd / 256;
  Data[1] = Tijd;
  Data[2] = digitalRead(8);

  LogWrite(Data); // schrijf naar log
}

//-----------------------------------------------------------------------------
// DemoLogRead - Routine om (via menu) LogRead te demonstreren
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DemoLogRead()
{ byte Data[LOG_MAX_DATA];
  
  LogReadOpen();  // lees vanaf 1e record

  for (;;) {
    bool r = LogRead(Data);

    if (r) {
      // success
      int Tijd = Data[0] * 256L + Data[1];
      int Pin8 = Data[2];
      printf("Tijd: %d sec, pin8: %d\n", Tijd, Pin8);    
    } 
    else {
      printf("Log end.\n");
      return;
    }
  }
}

//-----------------------------------------------------------------------------
// DemoPrintTijd - Lees tijd van RTC en print deze.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DemoPrintTijd()
{
  byte I2cTxBuffer[1];
  byte I2cRxBuffer[3];

  // print tijd
  I2cTxBuffer[0] = 0;     // register nummer

  bool r = I2cSendReceive(DS3231_I2C_ADDRESS, 1, 3, I2cTxBuffer, I2cRxBuffer);
  if (r) 
    printf("Tijd: %02x:%02x:%02x\n", I2cRxBuffer[2], I2cRxBuffer[1], I2cRxBuffer[0]);        
  else
    I2cError(DS3231_I2C_ADDRESS, 1215);
}

//----------------------------------------------------------------------------- 
// DemoSetTijd -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DemoSetTijd(int Uur, int Minuut, int Seconde) 
{
  byte I2cTxBuffer[4];

  I2cTxBuffer[0] = 0;           // register nummer
  I2cTxBuffer[1] = Bcd(Seconde);     
  I2cTxBuffer[2] = Bcd(Minuut);     
  I2cTxBuffer[3] = Bcd(Uur);     
  bool r = I2cSendReceive(DS3231_I2C_ADDRESS, 4, 0, I2cTxBuffer, NULL);  
  if (r) 
    DemoPrintTijd();
  else
    I2cError(DS3231_I2C_ADDRESS, 1216);
}

//-----------------------------------------------------------------------------
// DemoRunningLight -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DemoRunningLight(int count)
{
  byte I2cTxBuffer[1];

  for (int j=0; j<count; j++) {
    I2cTxBuffer[0] = 255;
    for (int i=0; i<8; i++) {
      I2cSendReceive(PCF8574A_I2C_ADDRESS, 1, 0, I2cTxBuffer, NULL);
      I2cTxBuffer[0] = I2cTxBuffer[0] * 2;
      delay(100);
    }
    for (int i=0; i<8; i++) {
      I2cTxBuffer[0] = I2cTxBuffer[0] * 2 + 1;
      I2cSendReceive(PCF8574A_I2C_ADDRESS, 1, 0, I2cTxBuffer, NULL);
      delay(100);
    }
  }
}

//-----------------------------------------------------------------------------
// DemoBlink -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DemoBlink()
{ 
  int r = 0;

  // Led aan
  Wire.beginTransmission(PCF8574A_I2C_ADDRESS);
  Wire.write(0);  
  r += Wire.endTransmission();          

  delay(1000);

  // Led uit
  Wire.beginTransmission(PCF8574A_I2C_ADDRESS);
  Wire.write(255);  
  r += Wire.endTransmission();    
  if (r != 0) I2cError(PCF8574A_I2C_ADDRESS, 1217);
}


byte EePattern[] = { 
  'D','e',' ','w','o','r','k','s','h','o','p',' ','v','a','n',' ','K','a','r','e','l',' ','e','n',' ','J','o','e','p','.',0,0,  
  0xde, 0xc0, 0xde, 0x00, 0xab, 0xba, 0xba, 0xbe,
  0x00, 0xca, 0xfe, 0xf0, 0x0d, 0x00, 0xbe, 0xef,
  0x00, 0xc0, 0xff, 0xee, 0xfe, 0xed, 0x00, 0xfa,
  0xca, 0xde, 0x00, 0xde, 0xad, 0x00, 0xc0, 0xde  }; 
void DemoEepromPattern()
{
  byte I2cTxBuffer[18];

  if (AddressProbe(EEPROM_I2C_ADDRESS) == false) { 
    I2cError(EEPROM_I2C_ADDRESS, 1218);
    return;
  }    

  for (int i=0; i<64; i+=16) {
    I2cTxBuffer[0] = 0;
    I2cTxBuffer[1] = i;    
    memcpy(I2cTxBuffer+2, EePattern + i, 16);
    I2cSendReceive(EEPROM_I2C_ADDRESS, 18, 0, I2cTxBuffer, NULL);
    while (AddressProbe(EEPROM_I2C_ADDRESS) == false) { 
      // wait for eeprom to finish write
    }    
  }  

}

void DemoEepromDump()
{
  byte I2cTxBuffer[2];
  byte I2cRxBuffer[16];

  for (int i=0; i<64; i+=16) {
    I2cTxBuffer[0] = 0;
    I2cTxBuffer[1] = i;    
    I2cSendReceive(EEPROM_I2C_ADDRESS, 2, 16, I2cTxBuffer, I2cRxBuffer);
    HexDump(I2cRxBuffer, 16, i);
  }  
}

// edump - demo code
void edump()
{
  byte I2cTxBuffer[2];
  byte I2cRxBuffer[16];

  for (int a=0; a<64; a+=16) {

    // Stuur gewenste adres
    I2cTxBuffer[0] = 0;
    I2cTxBuffer[1] = a;        
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);
    for (int i=0; i<2; i++) { 
      Wire.write(I2cTxBuffer[i]); // 
    }
    int r = Wire.endTransmission();  
    if (r != 0) return;  // error

    // lees 16 data bytes
    Wire.requestFrom(EEPROM_I2C_ADDRESS, 16);
    if (Wire.available() != 16) return;  // error

    // Plaats data in de buffer
    for (int i=0; i<16; i++) { 
      I2cRxBuffer[i] = Wire.read(); 
    }

    // Dump buffer
    HexDump(I2cRxBuffer, 16, a);
  }  
}





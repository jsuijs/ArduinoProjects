//-----------------------------------------------------------------------------
// I2CmTk.ino - I2C Master Toolkit (c) 2016-2017 Karel Dupain & Joep Suijs
// (version with demo code stripped)
//-----------------------------------------------------------------------------
// Deze toolkit is ontwikkeld voor de Workshop 'Arduino & I2C'.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//-----------------------------------------------------------------------------

static void(* resetFunc) (void) = 0;//declare reset function at address 0

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
    CSerial.printf( "Cnt = %d  A=%u  B=%u  C=%u  Cmd=%s\r\n", ParamCount, Number[0], Number[1], Number[2], Cmd) ;
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
    if (MatchCommand("debug",     1)) Lpp.I2cDebug = Number[0];
    if (MatchCommand("scan",      0)) BusScan();
    if (MatchCommand("fill",      3)) EepromFill(Number[0], Number[1], Number[2]);

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

    if (MatchCommand("reset",     0)) resetFunc();


    // commands below match LidarPreprocessor commands from RobotLib.
    // (with some short versions added)
    if (MatchCommand("start",    0))   Lpp.Start();
    if (MatchCommand("stop",     0))   Lpp.Stop();
    if (MatchCommand("silent",   0))   PeriodicInterval = 0;
    if (MatchCommand("interval", 1))   PeriodicInterval = Number[0];

    if (MatchCommand("offset",   1))   Lpp.SetOffsetDegrees(Number[0]);
    if (MatchCommand("reverse",  1))   Lpp.SetReverse(Number[0]);

    if (MatchCommand("setarray",   3)) Lpp.SensorSetup(Number[0], Number[1], Number[2]);
    if (MatchCommand("setsensor",  3)) Lpp.SensorSetup(Number[0], Number[1], Number[2]);
    if (MatchCommand("printsensors",  0) || MatchCommand("ps",  0)) {
      Lpp.ReadSensors();
      Lpp.PrintSensors();
    }
    if (MatchCommand("printarray",  0) || MatchCommand("pa",  0)) {
      Lpp.ReadArray();
      Lpp.PrintArray();
    }
    if (MatchCommand("printstatus",  0)) {
      Lpp.ReadStatus();
      Lpp.PrintStatus();
    }
    if (MatchCommand("printcfg",  0))  Lpp.ReadPrintSensorCfg(Number[0]);


    switch(LastError) {
    case 2 :
      {  // unknown command
        if (Cmd[1] == 0) {
          // single char keyword => return char
          return Cmd[0];
        }
        else {
          CSerial.printf("Onbekend cmd '%s' (2)\n", Cmd);
          return 2;
        }
        break;
      }
    case 1 :
      {
        CSerial.printf("Error: # params - cmd '%s'\n", Cmd);
        return 2;
      }
    case 0 :
      {
        CSerial.printf("TkCmd '%s' gereed.\n", Cmd);
        return 1;
      }
    default :
      {
        CSerial.printf("Error 0924\n");
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
    CSerial.setTimeout(50);
  }

  if( CSerial.available() > 0)
  {
    int r = Command.GetLine(CSerial.read());
    if (r == 0) return 0;  // still reading line

    if (r < 0) {
      CSerial.printf("Cmd parse err %d\n", r);
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

//-----------------------------------------------------------------------------
// PrintTkMsg - print I2C Master Toolkit message
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void PrintTkMsg()
{
  CSerial.print(F("\nI2CmTk - I2C Master Toolkit versie 0.81\n"));
  CSerial.print(F("Ontwikkeld voor de Workshop 'Arduino & I2C'.\n"));
  CSerial.print(F("(c) 2016-2017 Karel Dupain & Joep Suijs\n"));
  CSerial.printf("Gecompileerd: %s %s\n", __DATE__, __TIME__ );
}

//-----------------------------------------------------------------------------
// AddressProbe - check if slave is present.
//-----------------------------------------------------------------------------
// Return: true when slave is present
//-----------------------------------------------------------------------------
bool AddressProbe(int I2cSlaveAddress)
{
  MyWire.requestFrom(I2cSlaveAddress, 1);    // request 2 bytes from slave device #112

  if (MyWire.available() != 0) {
    MyWire.read();
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
      CSerial.printf("%2x", I2cSlaveAddress);
    }
    CSerial.printf(".");
    if ((I2cSlaveAddress & 0x01F) == 0x1F) CSerial.printf("\n");
  }
  CSerial.printf("\n");
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

  CSerial.printf("I2c read 0x%02x, ", Slave);
  if (RegBytes != 0) {
    CSerial.printf("Reg: %d (0x%02x), Data: %u (0x%x)\n", RegAddr, RegAddr, Data, Data);
  }
  else {
    CSerial.printf("Data: %u\n", Data);
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

  CSerial.printf("Slave: 0x%02x, ", Slave);
  if (RegBytes != 0) {
    CSerial.printf("register %d (0x%02x) set to %d (0x%02x)\n", RegAddr, RegAddr, Data, Data);
  }
  else {
    CSerial.printf("Data: %d (0x%02x)\n", Data, Data);
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
    CSerial.printf("#bytes limited to 32!\n");
    DataBytes = 32;
  }

  _SetupRegisterAddress(TxBuffer, RegBytes, RegAddr);

  if (!I2cSendReceive(Slave, RegBytes, DataBytes, TxBuffer, RxBuffer)) {
    CSerial.printf("I2cRead() - comms err\n");
    return;
  }
  HexDump(RxBuffer, DataBytes);
}

//-----------------------------------------------------------------------------
// EepromFill - Fill in 16 byte chuncks
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EepromFill(int Slave, int Size, int Value)
{
  byte TxBuffer[34];
  bool r;

  delay(10);  // allow pending write cycle to finish

  // fill txbuffer with data
  for (int i=2; i<18; i++) {
    TxBuffer[i] = Value;
  }

  for (int i=0; i<Size; i+= 16) {
    TxBuffer[0] = i >> 8;
    TxBuffer[1] = i & 0xFF;
    r = I2cSendReceive(Slave, 18, 0, TxBuffer, NULL);
    if (!r) {
      CSerial.printf("\nEeFill err %d.\n", i);
      return;
    }

    if ((i & 0x3F) == 0)  CSerial.printf(".");  // print dot in one of 64 runs.

    while (AddressProbe(Slave) == false) {
      // wait for eeprom to finish write
    }
  }
  CSerial.printf("\nEeFill done\n");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void I2cError(int Slave, int ErrorNr)
{
  CSerial.printf("I2c error bij communicatie met slave 0x%02x (%d).\n", Slave, ErrorNr);
}
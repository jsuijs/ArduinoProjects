//-----------------------------------------------------------------------------
// I2CmTk.ino - I2C Master Toolkit (c) 2016-2021 Karel Dupain & Joep Suijs
// (version with demo code stripped)
//-----------------------------------------------------------------------------
// Deze toolkit is ontwikkeld voor de Workshop 'Arduino & I2C'.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//-----------------------------------------------------------------------------

int I2cDebug = 0;

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

//-----------------------------------------------------------------------------
// PrintTkMsg - print I2C Master Toolkit message
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void PrintTkMsg()
{
  MyPrintf("\nI2CmTk\n");
  MyPrintf("Gecompileerd: %s %s\n", __DATE__, __TIME__ );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void I2cError(int Slave, int ErrorNr)
{
  MyPrintf("I2c error %d, slave: 0x%02x\n", ErrorNr, Slave);
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
// Execute - execute commando
//-----------------------------------------------------------------------------
// Called via CmdTakt() when a command is received from the serial port.
//-----------------------------------------------------------------------------
void Execute(int Param[])
{
   if (Command.Match("?",         0)) Command.Help("I2cToolkit command parser.");

   if (Command.Match("debug",     1)) I2cDebug = Param[0];
   if (Command.Match("scan",      0)) BusScan();

   if (Command.Match("rnb",       1)) I2cReader(Param[0], 0, 1,       0);
   if (Command.Match("rnw",       1)) I2cReader(Param[0], 0, 2,       0);
   if (Command.Match("rbb",       2)) I2cReader(Param[0], 1, 1, Param[1]);
   if (Command.Match("rbw",       2)) I2cReader(Param[0], 1, 2, Param[1]);
   if (Command.Match("rwb",       2)) I2cReader(Param[0], 2, 1, Param[1]);
   if (Command.Match("rww",       2)) I2cReader(Param[0], 2, 2, Param[1]);
   if (Command.Match("wnb",       2)) I2cWriter(Param[0], 0, 1,       0, Param[1]);
   if (Command.Match("wnw",       2)) I2cWriter(Param[0], 0, 2,       0, Param[1]);
   if (Command.Match("wbb",       3)) I2cWriter(Param[0], 1, 1, Param[1], Param[2]);
   if (Command.Match("wbw",       3)) I2cWriter(Param[0], 1, 2, Param[1], Param[2]);
   if (Command.Match("wwb",       3)) I2cWriter(Param[0], 2, 1, Param[1], Param[2]);
   if (Command.Match("www",       3)) I2cWriter(Param[0], 2, 2, Param[1], Param[2]);
   if (Command.Match("rn",        2)) I2cReader2(Param[0], 0, Param[1],       0);
   if (Command.Match("rb",        3)) I2cReader2(Param[0], 1, Param[2], Param[1]);
   if (Command.Match("rw",        3)) I2cReader2(Param[0], 2, Param[2], Param[1]);
}

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

int I2cDebug = 1; // 1 is normal operation (verbose), 2 = some debug, 3 = all debug
                  // 0 is silent (no output from commands => not toolkit/interactive mode...)

//-----------------------------------------------------------------------------
// I2cError - print message, slave ID and a code to support debugging...
//-----------------------------------------------------------------------------
// @ErrorNr: use the source...
//-----------------------------------------------------------------------------
void I2cError(int Slave, int ErrorNr)
{
  MyPrintf("Slave 0x%02x I2C error (%d).\n", Slave, ErrorNr);
}

//-----------------------------------------------------------------------------
// AddressProbe - check if slave is present.
//-----------------------------------------------------------------------------
// Return: true when slave is present
//-----------------------------------------------------------------------------
bool AddressProbe(int I2cSlaveAddress)
{
  Wire.requestFrom(I2cSlaveAddress, 1);    // request 2 bytes from slave device

  if (Wire.available() == 0)   return false;  // slave not present

  Wire.read();  // flush buffer
  return true;  // slave present
}

//-----------------------------------------------------------------------------
// BusScan - Scan i2c bus for slaves present.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BusScan()
{
  for (int I2cSlaveAddress = 0; I2cSlaveAddress<127; I2cSlaveAddress++) {

    if (AddressProbe(I2cSlaveAddress)) {
      MyPrintf("%2x ", I2cSlaveAddress);
    }
    MyPrintf(".");
    if ((I2cSlaveAddress & 0x01F) == 0x1F) MyPrintf("\n");
  }
  MyPrintf("\n");
}

//-----------------------------------------------------------------------------
// I2cSendReceive - Send and/or Receive data to/from i2c slave.
//-----------------------------------------------------------------------------
// return: true on success
//-----------------------------------------------------------------------------
bool I2cSendReceive(byte I2cSlaveAddress, byte TxCount, byte RxCount, const byte *TxBuffer, byte *RxBuffer)
{
  byte r;

  if (I2cDebug > 1) {
    MyPrintf("I2cSendReceive(%d %d %d)\n", I2cSlaveAddress, TxCount, RxCount);
  }

  if (TxCount > 0) {
    if (I2cDebug > 2) {
      MyPrintf("TxBuf:\n");
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
    if (I2cDebug > 2) {
      MyPrintf("RxBuf:\n");
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
      case 0 : { // no address
      }
      break;

      case 1 : { // byte address
         TxBuffer[0] = RegAddr;
       }
      break;

      case 2 : { // word address
         TxBuffer[0] = RegAddr >> 8;
         TxBuffer[1] = RegAddr;
      }
      break;

      default : { // invalid register size
         I2cError(0,100);
      }
      break;
   }
}

//-----------------------------------------------------------------------------
// I2cReader - support function for READ commands
//-----------------------------------------------------------------------------
// RegBytes:  nr of bytes, 0, 1 or 2, in RegAddr
// DataBytes: nr of bytes, 1 or 2, to be read
// Return:  data read (single value).
//          On error, -1 is returned. Note: -1 can also mean all went well and
//          the slave actual returned this value.
//-----------------------------------------------------------------------------
static int I2cReader(int Slave, int RegBytes, int DataBytes, int RegAddr)
{  unsigned int Data;
   bool r;
   byte TxBuffer[2], RxBuffer[2];

   _SetupRegisterAddress(TxBuffer, RegBytes, RegAddr);
   r = I2cSendReceive(Slave, RegBytes, DataBytes, TxBuffer, RxBuffer);

   if (!r) {
      I2cError(Slave, 201);
      return -1;
   }

   // process data.
   switch(DataBytes) {
      case 1 : { // byte data
         Data = RxBuffer[0];
      }
      break;

      case 2 : { // word data
         Data = RxBuffer[0];
         Data *= 256;
         Data += RxBuffer[1];
      }
      break;

      default : {
         I2cError(Slave, 202);
         return -1;
      }
      break;
   }

   if (I2cDebug != 0) {  // not in silent mode
      MyPrintf("I2c read 0x%02x, ", Slave);
      if (RegBytes != 0) {
         MyPrintf("Reg: %d (0x%02x), Data: %u (0x%x)\n", RegAddr, RegAddr, Data, Data);
      } else {
         MyPrintf("Data: %u\n", Data);
      }
   }
   return (int) Data;
}

//-----------------------------------------------------------------------------
// I2cWriter - support function for WRITE commands
//-----------------------------------------------------------------------------
// RegBytes:  nr of bytes, 0, 1 or 2, in RegAddr
// DataBytes: nr of bytes, 1 or 2, to write
//-----------------------------------------------------------------------------
static void I2cWriter( int Slave, int RegBytes, int DataBytes, int RegAddr, int Data)
{  bool r = 1;
   byte TxBuffer[4];

   _SetupRegisterAddress(TxBuffer, RegBytes, RegAddr);

   // Setup data.
   switch(DataBytes) {
      case 1 : { // byte data
         TxBuffer[RegBytes+0] = Data;
      }
      break;

      case 2 : { // word data
         TxBuffer[RegBytes+0] = Data/256;
         TxBuffer[RegBytes+1] = Data;
      }
      break;

      default : {
        I2cError(Slave, 301);
      }
      break;
  }

  r = I2cSendReceive(Slave, RegBytes+DataBytes, 0, TxBuffer, NULL);

  if (!r) {
    I2cError(Slave, 302);
    return;
  }

   if (I2cDebug != 0) {  // not in silent mode
      MyPrintf("Slave: 0x%02x, ", Slave);
      if (RegBytes != 0) {
         MyPrintf("register %d (0x%02x) set to %d (0x%02x)\n", RegAddr, RegAddr, Data, Data);
      } else {
         MyPrintf("Data: %d (0x%02x)\n", Data, Data);
      }
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
    MyPrintf("#bytes limited to 32!\n");
    DataBytes = 32;
  }

  _SetupRegisterAddress(TxBuffer, RegBytes, RegAddr);

  if (!I2cSendReceive(Slave, RegBytes, DataBytes, TxBuffer, RxBuffer)) {
    MyPrintf("I2cRead() - comms err\n");
    return;
  }
  HexDump(RxBuffer, DataBytes);
}

//-----------------------------------------------------------------------------
// TkExecute - execute toolkit commands
//-----------------------------------------------------------------------------
// Called via CmdTakt() when a command is received from the serial port.
//-----------------------------------------------------------------------------
void TkExecute(int Param[])
{
   if (Command.Match("?",     0)) Command.Help("I2cToolkit.");

   if (Command.Match("debug", 1)) I2cDebug = Param[0];
   if (Command.Match("scan",  0)) BusScan();

   if (Command.Match("rnb",   1)) I2cReader(Param[0], 0, 1,       0);
   if (Command.Match("rnw",   1)) I2cReader(Param[0], 0, 2,       0);
   if (Command.Match("rbb",   2)) I2cReader(Param[0], 1, 1, Param[1]);
   if (Command.Match("rbw",   2)) I2cReader(Param[0], 1, 2, Param[1]);
   if (Command.Match("rwb",   2)) I2cReader(Param[0], 2, 1, Param[1]);
   if (Command.Match("rww",   2)) I2cReader(Param[0], 2, 2, Param[1]);

   if (Command.Match("wnb",   2)) I2cWriter(Param[0], 0, 1,       0,  Param[1]);
   if (Command.Match("wnw",   2)) I2cWriter(Param[0], 0, 2,       0,  Param[1]);
   if (Command.Match("wbb",   3)) I2cWriter(Param[0], 1, 1, Param[1], Param[2]);
   if (Command.Match("wbw",   3)) I2cWriter(Param[0], 1, 2, Param[1], Param[2]);
   if (Command.Match("wwb",   3)) I2cWriter(Param[0], 2, 1, Param[1], Param[2]);
   if (Command.Match("www",   3)) I2cWriter(Param[0], 2, 2, Param[1], Param[2]);

   if (Command.Match("rn",    2)) I2cReader2(Param[0], 0, Param[1],       0);
   if (Command.Match("rb",    3)) I2cReader2(Param[0], 1, Param[2], Param[1]);
   if (Command.Match("rw",    3)) I2cReader2(Param[0], 2, Param[2], Param[1]);
}

//---------------------------------------------------------------------
// I2cSlaveRegisters.ino - Eenvoudige register interface
//---------------------------------------------------------------------
// Deze routines geven een master via i2c toegang tot de array
// I2cRegister[] via de volgende stappen:
//
// * Selecteer het gewenste register met een 1-byte adres.
//   let op: repeated start wordt niet ondersteund!
//
// * Schrijf 1 of meer registers na elkaar (max 31)
//of
// * Lees 1 of meer registers na elkaar (maximaal 32)
//
// Verder:
// * Update desgewenst bepaalde registers continue in de hoofdlus. Let
//   op dat dit interrupt-safe gebeurt als je sets van data hebt (zoals
//   integers)
//
// * I2cNewDataAvailable() geeft aan dat 1 of meer registers is beschreven.
//   Naar aanleiding hiervan kan data van master worden gebruikt. Maar je
//   kunt ook continue de waarden van de registers gebruiken.
//---------------------------------------------------------------------
#include <Arduino.h>
#include <Wire.h>

#ifndef SLAVE_WIRE
   #define SLAVE_WIRE Wire
#endif

void I2cTxEvent();
void I2cRxEvent(int bytesReceived);

volatile int TxEventCounter;
volatile int RxEventCounter;

volatile byte I2cRegister[REG_MAP_SIZE];
volatile bool _I2cNewDataFlag = false;
int I2cAddressPointer = 0;

void HexDump(const void *Data, int Length);
void HexDump(const void *Data, unsigned int Length, unsigned int Offset);
//---------------------------------------------------------------------
// I2cNewDataAvailable - return true when master has written to register(s).
//---------------------------------------------------------------------
// Return true if newdata (only once, since it clears the flag)
//---------------------------------------------------------------------
bool I2cNewDataAvailable()
{
   bool r = _I2cNewDataFlag;
   _I2cNewDataFlag = false;
   return r;
}
//---------------------------------------------------------------------
// I2cSlaveRegistersInit - Start SLAVE_WIRE & setup interrupt handlers
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void I2cSlaveRegistersInit(int Slave)
{
   SLAVE_WIRE.begin(Slave);
   SLAVE_WIRE.onRequest(I2cTxEvent);
   SLAVE_WIRE.onReceive(I2cRxEvent);

   memset((void *)I2cRegister, 0, REG_MAP_SIZE);
}

//---------------------------------------------------------------------
// RegisterDump - for debug
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void RegisterDump()
{
  CSerial.printf("RegisterDump: ");
   for (int i=0; i<REG_MAP_SIZE; i++) {
      CSerial.printf("%02x ", I2cRegister[i]);
   }
  CSerial.printf("\n");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ValidRegister(int Index, int Size)
{
   if ((Index + Size - 1) < REG_MAP_SIZE) return true;

   CSerial.printf("Register # out of range (%d, %d, %d)\n", Index, Size, REG_MAP_SIZE);
   return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int RegisterGetWord(int i)
{
   if (!ValidRegister(i, 2)) return -1;

   noInterrupts();
   short int Value = (((int) I2cRegister[i]) << 8) | (int) I2cRegister[i+1];
   interrupts();

   return Value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int RegisterGetByte(int i)
{
   if (!ValidRegister(i, 1)) return -1;
   return I2cRegister[i];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RegisterSetWord(int i, int v)
{
   if (!ValidRegister(i, 2)) return;

   noInterrupts();
   I2cRegister[i]    = v >> 8;
   I2cRegister[i+1]  = v;
   interrupts();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RegisterSetByte(int i, int v)
{
   if (!ValidRegister(i, 1)) return;
   I2cRegister[i] = v;
}

//---------------------------------------------------------------------
// I2cTxEvent - Setup data for transmit (interrupt handler)
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void I2cTxEvent()
{

  TxEventCounter++;

  // Set ID value in first register
  I2cRegister[R_ID] = IDENTIFICATION; // ID register

  // assure address is valid.
  if (I2cAddressPointer >= REG_MAP_SIZE) I2cAddressPointer = 0;
  if (I2cAddressPointer < 0)             I2cAddressPointer = 0;

  int Len = REG_MAP_SIZE-I2cAddressPointer;
  if (Len > 32) Len = 32;  // max # bytes in one transmission (arduino limit)
// disable print to avoid delay in SEND
//  printf("Tx %p %d\n", &I2cRegister[I2cAddressPointer], Len);
  // Allow reading of regeister from current address to end
  SLAVE_WIRE.write((const byte *) &I2cRegister[I2cAddressPointer], Len);
}

//---------------------------------------------------------------------
// I2cRxEvent - process received data (interrupt handler)
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void I2cRxEvent(int bytesReceived)
{
  int t;

  RxEventCounter++;

// disable print to avoid delay in SEND
  //CSerial.printf("rx %d ", bytesReceived);

  for (int a = 0; a < bytesReceived; a++) {
    t = SLAVE_WIRE.read();
//     CSerial.printf("%d ", t);
    if (a == 0) {
      I2cAddressPointer = t;
    }
    else {

      // assure address is valid.
      if (I2cAddressPointer >= REG_MAP_SIZE) I2cAddressPointer = 0;
      if (I2cAddressPointer < 0)             I2cAddressPointer = 0;

      I2cRegister[I2cAddressPointer] = t;  // store data
      I2cAddressPointer ++;                // prepare pointer for next write (or read)
      _I2cNewDataFlag = true;              // signal new data to main program
    }
  }
}

//-----------------------------------------------------------------------------
// RegDump - Dump all registers.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RegDump()
{
  HexDump((const void*)I2cRegister, REG_MAP_SIZE);
}

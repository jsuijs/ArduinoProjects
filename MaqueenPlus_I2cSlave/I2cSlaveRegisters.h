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
// * I2cNewData() geeft aan dat 1 of meer registers is beschreven. Naar
//   aanleiding hiervan kan data van master worden gebruikt. Maar je
//   kunt ook continue de waarden van de registers gebruiken.
//---------------------------------------------------------------------

#include <Arduino.h>
#include <Wire.h>

#ifndef SLAVE_WIRE
#define SLAVE_WIRE Wire
#endif

void requestEvent();
void receiveEvent(int bytesReceived);

byte I2cRegister[REG_MAP_SIZE];
bool _I2cNewDataFlag = false;
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
  SLAVE_WIRE.onRequest(requestEvent);
  SLAVE_WIRE.onReceive(receiveEvent);
}

//---------------------------------------------------------------------
// RegisterDump - for debug
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void RegisterDump()
{
  printf("RegisterDump: ");
  for (int i=0; i<REG_MAP_SIZE; i++) {
    printf("%02x ", I2cRegister[i]);
  }
  printf("\n");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ValidRegister(int Index, int Size)
{
   if ((Index + Size - 1) < REG_MAP_SIZE) return true;

   printf("Register # out of range (%d, %d, %d)\n", Index, Size, REG_MAP_SIZE);
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
// requestEvent - Setup data for transmit (interrupt handler)
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void requestEvent()
{
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
// receiveEvent - process received data (interrupt handler)
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void receiveEvent(int bytesReceived)
{
  int t;

  // disable print to avoid delay in SEND
  //  printf("rx 0x%02x 0x%02x\n", bytesReceived, TWSR);

  for (int a = 0; a < bytesReceived; a++) {
    t = SLAVE_WIRE.read();
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// *** vanaf hier een aantal routines uit de I2C Master Toolkit ***
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// my_putc - links stdout (printf) to serial
//-----------------------------------------------------------------------------
// put 'fdevopen( &my_putc, 0);' in setup();
//-----------------------------------------------------------------------------
int my_putc(char c, FILE __attribute__ ((unused)) *t) {
  if (c == '\n') Serial.write('\r');
  return Serial.write(c);
}

#ifndef HEXDUMP_DEFINED
#define HEXDUMP_DEFINED
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
#endif

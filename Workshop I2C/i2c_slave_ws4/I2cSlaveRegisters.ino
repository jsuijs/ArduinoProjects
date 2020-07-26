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
void requestEvent();
void receiveEvent(int bytesReceived);

bool _I2cNewDataFlag = false;
int I2cAddressPointer = 0;

//---------------------------------------------------------------------
// I2cNewData - return true when master has written to register(s).
//---------------------------------------------------------------------
// Return true if newdata (only once, since it clears the flag)
//---------------------------------------------------------------------
bool I2cNewData()
{
  bool r = _I2cNewDataFlag;
  _I2cNewDataFlag = false; 
  return r;
}

//---------------------------------------------------------------------
// I2cSlaveRegistersInit - Start Wire & setup interrupt handlers
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void I2cSlaveRegistersInit(int Slave)
{
  Wire.begin(Slave);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

//---------------------------------------------------------------------
// RegisterDump - for debug
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void RegisterDump()
{
  if (I2cNewData()) {
    printf("RegisterDump: ");
    for (int i=0; i<REG_MAP_SIZE; i++) {
      printf("%02x ", I2cRegister[i]);
    }
    printf("\n"); 
  }
}

//---------------------------------------------------------------------
// requestEvent - Setup data for transmit (interrupt handler)
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void requestEvent()
{
  // assure address is valid.
  if (I2cAddressPointer >= REG_MAP_SIZE) I2cAddressPointer = 0;
  if (I2cAddressPointer < 0)             I2cAddressPointer = 0;

  int Len = REG_MAP_SIZE-I2cAddressPointer;
  if (Len > 32) Len = 32;  // max # bytes in one transmission (arduino limit)
  // disable print to avoid delay in SEND
  //  printf("Tx %p %d\n", &I2cRegister[I2cAddressPointer], Len);
  // Allow reading of regeister from current address to end
  Wire.write((const byte *) &I2cRegister[I2cAddressPointer], Len); 
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
    t = Wire.read();
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
int my_putc(char c, FILE *t) {  
  if (c == '\n') Serial.write('\r');
  return Serial.write(c);
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




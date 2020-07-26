// I2cSoftware

// prototypes                   
void _i2c_wait();
bool _i2c_bit_in();
void _i2c_bit_out(bool x);                  
void  I2cStart();
void  I2cStop();

#define PIN_SCL A5
#define PIN_SDA A4

void I2cScl(bool In)
{
  if (In) {
    // Zet SCL hoog (input)
    pinMode(PIN_SCL, INPUT); 
    _i2c_wait();       

    // Wacht tot SCL hoog is
    while (digitalRead(PIN_SCL) == LOW) { /* wait */ }
    
  } else {
    // Zet SCL laag
      digitalWrite(PIN_SCL, LOW); 
      pinMode(PIN_SCL, OUTPUT); 
      digitalWrite(PIN_SCL, LOW); 
  }
    _i2c_wait();       
}

void I2cSda(bool In)
{
  if (In) {
    // Zet SDA hoog (input)
    pinMode(PIN_SDA, INPUT); 
  } else {
    // Zet SDA laag
    digitalWrite(PIN_SDA, LOW); 
    pinMode(PIN_SDA, OUTPUT);     
    digitalWrite(PIN_SDA, LOW); 
  }
   _i2c_wait();         
}

bool I2cSdaRead()
{
   _i2c_wait();

  return true;  // todo - return SDA level
}

// ------------------------------------------------------------------------
// i2c_initialize - initialize the software i2c bus
// ------------------------------------------------------------------------
// The output latch values are always low,
// the lines are switched open-collector fashion by 
// manipulating the direction registers.
// ------------------------------------------------------------------------
void I2cInitialize() 
{
  I2cSda(HIGH);
  I2cScl(HIGH);

   
   // Clear the bus, regardless the state it is in.
   //
   // This code is based on atmel AT24C32C datasheet, page 6
   // http://www.atmel.com/dyn/resources/prod_documents/doc5298.pdf
   //
   I2cStart();
   for (int x=0; x<9; x++) {
     _i2c_bit_out(1);
   }
  I2cStart();
  I2cStop();  
}

// ------------------------------------------------------------------------
// i2c_start - output a start condition
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void I2cStart() 
{
   _i2c_wait();
   I2cSda(HIGH);       // data high
   I2cScl(HIGH);       // clock high

   I2cSda(LOW);        // data low
   I2cScl(LOW);       // clock low
}

// ------------------------------------------------------------------------
// i2c_stop - output a stop condition
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void I2cStop() 
{
   _i2c_wait();
   I2cSda(LOW);        // data low
   I2cScl(LOW);        // clock low
   
   I2cScl(HIGH);       // clock high
   I2cSda(HIGH);       // data high    
}

// ------------------------------------------------------------------------
// I2cWrite - output one byte
// ------------------------------------------------------------------------
// return true = okay, false = no ack received
// ------------------------------------------------------------------------
bool I2cWrite( byte  x) 
{
   for (int y=0; y<8; y++) {
      _i2c_bit_out(x & 0x80);
      x = x << 1;   
   }
   
   // wait for an ack condition   
   //procedure  _i2c_wait()_ack is
   _i2c_wait();
   I2cSda(HIGH);       // data open
   I2cScl(HIGH);       // clock high
      
   // check for the slave's acknowledge 
   bool r = ! I2cSdaRead();
   _i2c_wait(); 
   I2cScl(LOW);        // clock low
   
   return r; // true = okay, false = no ack received
}

// ------------------------------------------------------------------------
// I2cRead - read one byte
// ------------------------------------------------------------------------
// The ACK flag indicated if the byte read must be acked.
//
// In general, all bytes read by the master are acked, except the last one.
// Failing to NACK the last byte read will give unpredictable results.
// (Often it will provide repeatable sequences, where one out of 2 to 4 of 
// the reads from a specific device is correct. You are warned!)
// ------------------------------------------------------------------------
byte I2cRead(bool ack)
{  byte ret;
   
   //var bit b at ret : 0
   for (int x=0; x<8; x++) {
      ret = ret << 1;
      if (_i2c_bit_in()) ret |= 1;
   }
   
   if (ack) {
      _i2c_bit_out(LOW); // ack
   } else {
      _i2c_bit_out(HIGH); // nack
   }

   return ret;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// internal 
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// _i2c_wait - wait for a change to settle (for internal use only)
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void _i2c_wait()
{  
    // i2c_100kHz
    delay(500);
    return;
}

// ------------------------------------------------------------------------
// _i2c_bit_out- output a single bit (for internal use only)
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void _i2c_bit_out(bool x) 
{   
   I2cSda(x);          // high data bit
   I2cScl(HIGH);        // clock high   
   I2cScl(LOW);         // clock low
}

// ------------------------------------------------------------------------
// _i2c_bit_in- input a single bit (for internal use only)
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool _i2c_bit_in() 
{
   _i2c_wait();
   I2cSda(HIGH); // data open
   I2cScl(HIGH);  // clock high

   bool x = I2cSdaRead();                // sample data
   I2cScl(LOW);   // clock low
   return x;
}


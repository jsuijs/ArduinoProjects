
//---------------------------------------------------------
// i2c stuff start
// A4/A5 - i2c pins SDA/SCL on Uno

// I2c registers:
#define R_CMD           0    // 7/8/* van toetsenbord, 0 = done -> Morse patroon H C R
#define R_LED_M         1    // Waarde slave D8 -> LED master (D13)
#define R_ANALOG_H      2    // Hi-byte van slave analoge ingang A0 -> Master
#define R_ANALOG_L      3    // Lo-byte van slave analoge ingang A0 -> Master
#define R_LED_S         4    // 1/2 van toetsenbord -> LED slave (D13)
#define R_PWM           5    // 4/5 van toetsenbord = up/down -> PWM D9

#define SLAVE_ADDRESS   0x88/2  // slave address, any number from 0x01 to 0x7F

#define  REG_MAP_SIZE   (6)
volatile byte I2cRegister[REG_MAP_SIZE];
// i2c stuff end
//---------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  fdevopen( &my_putc, 0);    // Koppel printf uitvoer (stdout) via my_putc() aan de serial port.
  printf("Arduino slave ready");

  //---------------------------------------------------------
  // i2c interface init
  I2cSlaveRegistersInit(SLAVE_ADDRESS);
}

void loop()
{
  RegDump();

  // Oefening a

  // Oefening b

  if (I2cNewData()) {

    // Oefening c


    // Oefening d

  }

  // Oefening e

}






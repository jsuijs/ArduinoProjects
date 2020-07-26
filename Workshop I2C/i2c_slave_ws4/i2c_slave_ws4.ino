
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
  pinMode(8, INPUT_PULLUP);    // LED_M input pin
  pinMode(13, OUTPUT);         // LED_S output pin
}

void loop()
{
  RegDump();


  // Oefening a
  I2cRegister[R_LED_M] = digitalRead(8); // LED master volgt pin 8

  // Oefening b
  digitalWrite(13, I2cRegister[R_LED_S]);

  if (I2cNewData()) {

    // Oefening c
    analogWrite(9, I2cRegister[R_PWM]);

    // Oefening d
    byte cmd = I2cRegister[R_CMD];
    I2cRegister[R_CMD] = 0;  // wis commando

    if (cmd == 1) {   // PWM LED uit
      analogWrite(9, 0);
      delay(2000);
      analogWrite(9, I2cRegister[R_PWM]);
    }

    if (cmd == 2) {   // PWM LED laag
      analogWrite(9, 8);
      delay(2000);
      analogWrite(9, I2cRegister[R_PWM]);
    }

    if (cmd == 3) {   // PWM LED hoog
      analogWrite(9, 32);
      delay(2000);
      analogWrite(9, I2cRegister[R_PWM]);
    }

    if (cmd == 4) {   // PWM LED vol aan
      analogWrite(9, 255);
      delay(2000);
      analogWrite(9, I2cRegister[R_PWM]);
    }
  }

  // Oefening e
  int a = analogRead(A0);
  noInterrupts();
  // Deze code wordt niet onderbroken door de (i2c) interrupt routine.
  I2cRegister[R_ANALOG_H] = a / 256;
  I2cRegister[R_ANALOG_L] = a;
  interrupts();
}






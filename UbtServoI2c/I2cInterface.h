// i2c interface definition (project specific)

//--------------------
// I2c registers START
//--------------------

// status values
#define R_ID         (0)   // fixed number
#define R_CMD        (1)   // Command to be executed
#define R_SERVO_ID   (2)   // Servo ID
#define R_PARAM0     (3)   // cmd-dependant parameter
#define R_PARAM1     (4)   //
#define R_PARAM2     (5)   //
#define R_PARAM3     (6)   //

#define R_RET0       (8)   // 1th return-byte (high byte if a short is returned)
#define R_RET1       (9)   // 2nd return-byte (low byte if a short is returned)

//------------------
// I2c registers END
//------------------

#define  REG_MAP_SIZE   (R_RET1+2)

int  RegisterGetWord(int i);
void RegisterSetWord(int i, int v);
int  RegisterGetByte(int i);
void RegisterSetByte(int i, int v);

extern unsigned char I2cRegister[];

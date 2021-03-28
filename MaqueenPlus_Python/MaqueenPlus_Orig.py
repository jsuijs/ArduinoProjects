# Class for MaqueenPlus with stock firmware.
from microbit import *
import struct

class MaqueenPlus :

   def __init__(self):
      self.I2caddr = 0x10

   # 1.5 mm / tick
   def GetEncoders(self):
       buf = bytearray(1)
       buf[0] = 0x04   # register nr
       i2c.write(self.I2caddr, buf)
       return struct.unpack('>HH', i2c.read(self.I2caddr, 4))

   def ResetEncoders(self):
       buf = bytearray(5)  # fill with zero
       buf[0] = 0x04   # register nr
       i2c.write(self.I2caddr, buf)

   def Motors(self, PwmL, PwmR):
       buf = bytearray(5)
       buf[0] = 0x00   # register nr
       if PwmL > 0 :
           buf[1] = 1
           buf[2] = PwmL
       else :
           buf[1] = 2
           buf[2] = -PwmL

       if PwmR > 0 :
           buf[3] = 1
           buf[4] = PwmR
       else :
           buf[3] = 2
           buf[4] = -PwmR

       i2c.write(self.I2caddr, buf)

   #Corresponding sensor parameter： L3:1, L2:2, L1:3, R1:4, R2:5, R3:6
   #Index: the corresponding sensor
   def line(self, index):
       buf = bytearray(1)
       buf[0] = 0x1D
       i2c.write(self.I2caddr, buf)
       line_d = struct.unpack('b', i2c.read(self.I2caddr, 1))
       make = 0
       if index == 1:
           if (line_d[0] & 0x01) == 1:
               make = 1
           else:
               make = 0
       elif index == 2:
           if (line_d[0] & 0x02) == 2:
               make = 1
           else:
               make = 0
       elif index == 3:
           if (line_d[0] & 0x04) == 4:
               make = 1
           else:
               make= 0
       elif index == 4:
           if (line_d[0] & 0x08) == 8:
               make = 1
           else:
               make = 0
       elif index == 5:
           if (line_d[0] & 0x10) == 16:
               make = 1
           else:
               make = 0
       elif index == 6:
           if (line_d[0] & 0x20) == 32:
               make = 1
           else:
               make = 0
       return make

   # PID parameters opeen:0 close:1
   def PID(self, switch):
       buf = bytearray(2)
       buf[0] = 0x0A
       buf[1] = switch
       i2c.write(self.I2caddr, buf)

   # Corresponding sensor parameter： L3:1, L2:2, L1:3, R1:4, R2:5, R3:6
   # Index: the corresponding sensor
   def grayscaleValue(self, index):
       buf = bytearray(1)
       buf[0] = 0x1E
       i2c.write(self.I2caddr, buf)
       grayscaleValue_d = struct.unpack('>HHHHHH', i2c.read(self.I2caddr, 12))
       if index == 1:
           return grayscaleValue_d[0]
       elif index == 2:
           return grayscaleValue_d[1]
       elif index == 3:
           return grayscaleValue_d[2]
       elif index == 4:
           return grayscaleValue_d[3]
       elif index == 5:
           return grayscaleValue_d[4]
       elif index == 6:
           return grayscaleValue_d[5]

   # Each number represents a color, and color out of the RGB color range cannot be displayed.
   # RGB color range 1~7
   def RGB(self, colourL, colourR):
       buf = bytearray(3)
       buf[0] = 0x0b
       buf[1] = colourL
       buf[2] = colourR
       i2c.write(self.I2caddr, buf)

   # Motor compensation is used to adjust small differences in speed between motors
   # direction:1=left motor direction:2=right motor
   # speed:0~255
   def mostotCompensation(self, direction, speed):
       if direction == 1:
           buf = bytearray(2)
           buf[0] = 0x08
           buf[1] = speed
           i2c.write(self.I2caddr, buf)
       elif  direction == 2:
           buf = bytearray(2)
           buf[0] = 0x09
           buf[1] = speed
           i2c.write(self.I2caddr, buf)

   def servo(self, index, angle):
       buf = bytearray(2)
       buf[1]=angle

       if(index == 1):
           buf[0]=0x14
           i2c.write(self.I2caddr, buf)
       if(index == 2):
           buf[0]=0x15
           i2c.write(self.I2caddr, buf)
       if(index == 3):
           buf[0]=0x16
           i2c.write(self.I2caddr, buf)

if __name__ == "__main__":
   import music
   music.play(music.PYTHON[:4])
   Robot = MaqueenPlus()

   # start of main
   Robot.ResetEncoders()
   Robot.RGB(1, 2)
   while True :
       Enc = Robot.GetEncoders()
       Delta = (Enc[0] - Enc[1]) * 20  # heading error * kP
       Robot.Motors(100-Delta, 100+Delta)    # PWM 100 + compensate for heading error
       if (Enc[0] + Enc[1]) > 1333 : break # drive 1333 * 1.5 (mm/tick) / 2 (wheels) = 1 meter.

   Robot.Motors(0, 0)
   Robot.RGB(0,0)
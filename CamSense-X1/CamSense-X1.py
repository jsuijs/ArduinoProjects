#
# CamSense-X1 - Copyright (c) 2020 Joep Suijs - All rights reserved.
#
import serial, math, sys, os
from serial.tools import list_ports
from time import sleep

# lists to collect plot-values.
XValues = list()
YValues = list()

#------------------------------------------------------------------------------
def DecodeFrame(Message):
   global OutFp

   if args.debug :
      OutFp.write("".join("{:02x} ".format(x) for x in Message) + '\n')

   if ValidateFrame(Message) == False : return

   Speed = Message[4] + 256 * Message[5]
   StartAngle = (Message[6] + 256 * Message[7])  / 64 - 640
   EndAngle  = (Message[32] + 256 * Message[33]) / 64 - 640
   if EndAngle >= StartAngle :
      DeltaAngle = (EndAngle - StartAngle) / 8
   else :
      DeltaAngle = (EndAngle - (StartAngle - 360)) / 8

   if args.debug :
      OutFp.write("StartAngle: %f, End: %f, Delta: %f\n" % (StartAngle, EndAngle, DeltaAngle))

   for i in range(0, 7) :
      Base = 8 + i * 3
      Distance = Message[Base + 0] + 256 * Message[Base + 1]
      Quality  = Message[Base + 2]
      Angle = StartAngle + DeltaAngle * i
      #print("D", Angle, Quality, Distance)

      if Quality > 0 :
         X = math.cos(Angle / 57.3) * Distance
         Y = math.sin(Angle / 57.3) * Distance * -1
         OutFp.write("%d\t%d\t%d\t%d\t%d\n" % (Angle, Quality, Distance, X, Y));
         XValues.append(X)
         YValues.append(Y)

#------------------------------------------------------------------------------
def ValidateFrame(InMessage) :

   Message = InMessage.copy() # create copy, which we can empty for checksum calcuation

   # Note: first 4 bytes of message must be 55 aa 03 08, which is validated by the checksum

   if len(Message) != 36:
      print("Error: invalid message lenght, not processed");
      print(Message)
      return False   # validation failed

   # Checksum, included in the message
   MsgChecksum = Message[34] + 256 * Message[35]

   # Calculate checksum of message
   MyCheck = 0
   while len(Message) > 2 :
      Low      = Message.pop(0)
      High     = Message.pop(0)
      MyCheck  = MyCheck * 2 + (Low  + 256 * High)

   MyCheck &= 0x3FFFFFFF   # show 30 bit math is enough
   MyCheck = (MyCheck & 0x7FFF) + int (MyCheck / 32768)
   MyCheck &= 0x7FFF

   if MyCheck == MsgChecksum : return True   # success

   # checksum error
   print("Checksum error (%04x vs %04x)" % (MyCheck, MsgChecksum))
   print(InMessage)
   return False   # validation failed

#------------------------------------------------------------------------------
import numpy as np
import matplotlib.pyplot as plt

def PlotGrap(ShowGraph, Picture) :

   if len(XValues) == 0 :
      print("PlotGraph: no samples to show");
      return;

   print("PlotGraph for %d points" % len(XValues))
   x_array = np.array(XValues)
   y_array = np.array(YValues)

   fig, ax = plt.subplots()
   ax.scatter(x_array, y_array, s=1, color='blue') # s is marker size

   ax.set(xlabel='X', ylabel='Y',
          title='Scatter plot')
   ax.grid()

   # scale both axes to same range
   XRange  =  max(XValues) - min(XValues)
   XCenter = (max(XValues) + min(XValues)) / 2
   YRange  =  max(YValues) - min(YValues)
   YCenter = (max(YValues) + min(YValues)) / 2
   HRange  = max(XRange, YRange) / 2;
   ax.axis(xmin=XCenter - HRange, xmax= XCenter + HRange, ymin=YCenter - HRange, ymax= YCenter + HRange)
   #        self.a.axis(xmin=XCenter - HRange, xmax= XCenter + HRange, ymin=YCenter - HRange, ymax= YCenter + HRange)

   if Picture != None:
      fig.savefig(Picture + ".png")

   if ShowGraph :
      plt.show()

#------------------------------------------------------------------------------
def InputFromSerial(SerialPort, SerialBaud, MaxNrSamples) :
   global OutFp, RawFp

   # Open serial port
   print("Connect to SerialPort %s at %d" % (SerialPort, SerialBaud))

   try:
      ser = serial.Serial('\\\\.\\' + SerialPort, SerialBaud, timeout=0, write_timeout=0)
      ser.flushInput()

   except:
      ports = list_ports.comports()
      if len(ports) > 0 :
         print("Failed to connect serial, serial ports on this system:")
         ports.sort()
         for p in ports:
            print(p)
      else :
         print("Failed to connect serial - NO serial ports on this system.")
      raise SystemExit(99)

   # port open, now read data

   InSync = False
   ser.dtr = True   # activate relais, True is low at ttl level (not-dtr).

   #sleep(10)
   while True :

      # main loop - collect data from lidar & print.
      if  ser.inWaiting() > 40:

         if InSync :
            MsgData = ser.read(36)
            if MsgData[0] != 0x55 :
               print("Sync lost")
               InSync = False
               continue

            # dump raw frames
            RawFp.write("".join("{:02x} ".format(x) for x in MsgData) + '\n')

            DecodeFrame(list(MsgData))

            if MaxNrSamples > 0 :  # non-zero: limit amount of data to be collected.
               # XValues contains all valid samples from DecodeFrame.
               if len(XValues) >= MaxNrSamples :
                  print("Sample-count limit received, done.")
                  break
         else :
            # get in sync
            MsgData = ser.read(1)
            if MsgData[0] != 0x55 :
               continue;

            MsgData = ser.read(35)   # read reminder
            InSync = True

            # note: this record is not processed...

   ser.dtr = False   # de-activate relais

#------------------------------------------------------------------------------
def InputFromFile(InFile, MaxNrSamples) :

   print("Read from file '%s'" % InFile)

   with open(InFile) as fp:

      while True:
         line = fp.readline().strip().replace('\t', ' ')
         if not line : break # file done => exit

         fields = line.split()
         if len(fields) < 36 :
            print("incomplete:", len(fields), "fields, msg: '%s'" % line)
            print(fields)
            continue   # incomplete message

         #Numbers = [int(f, 16) for f in L.rstrip()[1:-1].split('][')]
         Numbers = [int(f, 16) for f in fields]

         DecodeFrame(Numbers)

         if MaxNrSamples > 0 :  # non-zero: limit amount of data to be collected.
            # XValues contains all valid samples from DecodeFrame.
            if len(XValues) >= MaxNrSamples :
               print("Sample-count limit received, done.")
               break

#------------------------------------------------------------------------------
# MAIN CODE
#------------------------------------------------------------------------------

# create parser
import argparse

parser = argparse.ArgumentParser()
#parser.add_argument('port', nargs='?', default='com15',  help='Com port (e.g. com15)')
parser.add_argument('-port',     default='com15',     help='specify Windows com-port (e.g. -port com15)')
parser.add_argument('-baud',     default=115200,      help='specify baudrate')
parser.add_argument('-count',    default=1000,        help='exit after <COUNT> samples, 0 = do not exit')

parser.add_argument('-infile',                        help='read raw data from <INFILE> (do not use com-port)')

parser.add_argument('-outfile',                       help='write decoded data to <OUTFILE>')
parser.add_argument('-rawfile',                       help='write raw data to <RAWFILE>')

parser.add_argument('-graph',    action='store_true', help='show graph of data')
parser.add_argument('-debug',    action='store_true', help='more output')
parser.add_argument('-picture',                       help='save graph as <PICTURE>.png')

args = parser.parse_args()

# setup output-file for parsed data
if args.outfile != None :
   print("Output in file '%s'" % args.outfile)
   OutFp = open(args.outfile, 'w+')
else :
   OutFp = sys.stdout

# setup output-file for raw data
RawFp = open(os.devnull, 'w')   # default: write raw to null device
if args.rawfile != None :
   if args.infile != None :
      print("Warning: --outfile is ignored when --infile is specified.")
   else :
      print("Output raw data to file '%s'" % args.rawfile)
      RawFp = open(args.rawfile, 'w+')

# get the data
if args.infile == None :
   InputFromSerial(args.port, args.baud, int(args.count))
else :
   InputFromFile(args.infile, int(args.count))

# show plot
if args.graph or args.picture != None:
   PlotGrap(args.graph, args.picture)

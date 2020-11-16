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
def PrintDistanceValues(Message):
   global OutFp

   Numbers = Message[2:] # drop 55 aa

   Speed = Numbers[2] + 256 * Numbers[3]
   StartAngle = (Numbers[4] + 256 * Numbers[5])  / 64 - 640
   EndAngle  = (Numbers[30] + 256 * Numbers[31]) / 64 - 640
   if EndAngle > StartAngle :
      DeltaAngle = (EndAngle - StartAngle) / 8
   else :
      DeltaAngle = (EndAngle - (StartAngle - 360)) / 8

   #print("StartAngle:", StartAngle, "End:", EndAngle, "Delta:", DeltaAngle);
   for i in range(0, 7) :
      Base = 6 + i * 3
      Distance = Numbers[Base + 0] + 256 * Numbers[Base + 1]
      Quality  = Numbers[Base + 2]
      Angle = StartAngle + DeltaAngle * i
      #print("D", Angle, Quality, Distance)

      if Quality > 0 :
         X = math.cos(Angle / 57.3) * Distance
         Y = math.sin(Angle / 57.3) * Distance * -1
         OutFp.write("%d\t%d\t%d\t%d\t%d\n" % (Angle, Quality, Distance, X, Y));
         XValues.append(X)
         YValues.append(Y)

#------------------------------------------------------------------------------
def CollectFromSerial(SerialPort, SerialBaud, MaxNrSamples) :
   global OutFp, RawFp

   # Open serial port
   print("Connect to SerialPort %s at %d" % (SerialPort, SerialBaud))

   try:
      ser = serial.Serial('\\\\.\\' + SerialPort, SerialBaud, timeout=0, write_timeout=0)
      ser.flushInput()

   except:
      print("Failed to connect serial, ports on this system:")
      ports = list_ports.comports()
      ports.sort()
      for p in ports:
         print(p)
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

            # collect non-zero samples
            PrintDistanceValues(MsgData)
            if MaxNrSamples > 0 :  # non-zero means MaxNrSamples limits amount of data to be collected.
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
import numpy as np
import matplotlib.pyplot as plt

def PlotGrap() :

   if len(XValues) == 0 :
      print("PlotGraph: no samples to show");
      return;

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

   fig.savefig("test.png")
   plt.show()

#------------------------------------------------------------------------------
# MAIN CODE
#------------------------------------------------------------------------------

# create parser
import argparse

parser = argparse.ArgumentParser()
#parser.add_argument('port', nargs='?', default='com15',  help='Com port (e.g. com15)')
parser.add_argument('--port',    default='com15',     help='Com port (e.g. com15)')
parser.add_argument('--baud',    default=115200,      help='Baudrate')
parser.add_argument('--count',   default=200,         help='Exit after <count> samples, 0 = do not exit')

parser.add_argument('--infile',                       help='Read raw data from file (do not use com-port)')

parser.add_argument('--outfile',                      help='Save to this file')
parser.add_argument('--rawfile',                      help='Output raw data to file')

parser.add_argument('--graph',   action='store_true', help='Show graph of data')

args = parser.parse_args()

print(args)

# setup output-files
if args.outfile != None :
   print("Output in file '%s'" % args.outfile)
   OutFp = open(args.outfile, 'w+')
else :
   OutFp = sys.stdout

# setup output-file
if args.rawfile != None :
   print("Output raw data to file '%s'" % args.rawfile)
   RawFp = open(args.rawfile, 'w+')
else :
   # write raw to null device
   RawFp = open(os.devnull, 'w')

# get the data
if args.infile == None :
   CollectFromSerial(args.port, args.baud, int(args.count))
else :
   print("todo read from file '%s'" % args.infile)

# show plot
if args.graph :
   PlotGrap()

# CamSense-X1
CamSense-X1 lidars are offered as surplus on AliExpress. These seem to be units that have been replaced by a service center. Despite this, more than half of them seem to work resonably well.
For more details on the lidar and it's protocol, see https://github.com/Vidicon/camsense-X1.

### Script functions
This script is used to test CamSense-X1 lidars, to diagnose issues and delve deeper in the sensor output data when anomalies are encountered.
The scripts can:
* capture Lidar data from the serial port
* save captured data to a file (-rawfile)
* read captured data from a file (-infile)
* output interpreted data in tab-separated format (to stdout or file specified by -outfile)
* show a graph for the interpreted data (-graph)
* save an image of the graph (-picture)

### Checksum calculation
The script also documents the checksum algorithm of the lidar:
```python
   # Checksum is calculated over 34 bytes, from 0x55 through endAngleH
   MyCheck = 0
   while len(Message) > 2 :
      Low      = Message.pop(0)
      High     = Message.pop(0)
      MyCheck  = MyCheck * 2 + (Low  + 256 * High)

   MyCheck = (MyCheck + int (MyCheck / 32768)) & 0x7FFF
```

This example shows how to seed first 4 (fixed) bytes and calculate the checksum on the remaining 30 bytes:
```C++
      // Calculate checksum
      int32_t CalcChecksum = 0xAA55 * 2 + 0x0803;  // seed with first four bytes (ll-hh order)
      for (int i=0; i< 30; i+=2) {
         // feed remaining 30 bytes (up to checksum)
         CalcChecksum = (CalcChecksum * 2) + ((((uint16_t)RxData[i+1]) << 8) | RxData[i]);
      }
      // Post processing: from 30 to 15 bits.
      uint16_t MyChecksum = 0x7FFF & ((CalcChecksum >> 15) + (CalcChecksum & 0x7FFF));
```

### Helptext
Just for reference, please check the script for the most up-to-date version.
```
usage: CamSense-X1.py [-h] [-port PORT] [-baud BAUD] [-count COUNT] [-infile INFILE] [-outfile OUTFILE]
                     [-rawfile RAWFILE] [-graph] [-debug] [-picture PICTURE]
optional arguments:
  -h, --help        show this help message and exit
  -port PORT        specify Windows com-port (e.g. -port com15)
  -baud BAUD        specify baudrate
  -count COUNT      exit after <COUNT> samples, 0 = do not exit
  -infile INFILE    read raw data from <INFILE> (do not use com-port)
  -outfile OUTFILE  write decoded data to <OUTFILE>
  -rawfile RAWFILE  write raw data to <RAWFILE>
  -graph            show graph of data
  -debug            more output
  -picture PICTURE  save graph as <PICTURE>.png
```
# Arduino_GPS_SD_Logger
Arduino sketch and library to track GPS CSV data onto an SD card for importing to google maps.
Program is built to utilize an analog light and motion sensor to detect tampering/opening of a box/enclosed truck.

In order for sketch to work you must place BuildMo_GPS_Library-main in C:\Users\UsersNameHere\Documents\Arduino\libraries.
BuildMo_GPS_Library is simply the original arduino GPS library that has been cut down to save on memory to accomodate sd.h for logging.

Board used is an Arduino Uno.


Pin Layouts are as follows:

GPS:
RX - Green  2
TX - BLUE 3

MSense (Motion Sensor):
Signal - BROWN A1

LSense (Light Sensor:
Signal - PURPLE  A0

SDCard:
MISO - BLACK  12
MOSI - WHITE  11
SCK/CLK - WHITE W TAPE 13
CS - PURPLE 3

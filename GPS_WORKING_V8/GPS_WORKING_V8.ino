/**************************************************************************/
/*
 * GPS:
RX - Green  2
TX - BLUE 3

MSense:
Sig - BROWN A1

LSense:
Sig - PURPLE  A0

SDCard:
MISO - BLACK  12
MOSI - WHITE  11
SCK/CLK - WHITE W TAPE 13
CS - PURPLE 3

*/
/**************************************************************************/
#include <SPI.h>
#include <SD.h>
#include <BuildMo_GPS.h>
#include <SoftwareSerial.h>

File myFile;

// If you are using hardware serial, make sure the two lines below are uncommented. (ARDUINO MEGA)
// #define GPSSerial Serial1
// BuildMo_GPS GPS(&GPSSerial);

// If you are using software serial, make sure the two lines below are uncommented. (ARDUINO UNO)
SoftwareSerial mySerial(3, 2);  // (Rx,Tx)
BuildMo_GPS GPS(&mySerial);

bool GPSfix;
float GPSlat, GPSlong, GPStime;
char Tamper;

int analogValueL = 0;
int analogInputPinL = A0;
int sensorPinL = A0;

int analogValueM = 0;
int sensorPinM = A1;
int analogInputPinM = A1;



void setup()
{
  GPS.begin(9600);      // Starts Serial comms with GPS module and initializes the module
  Serial.begin(9600);   // Starts Serial comms with the computer

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  pinMode(analogInputPinL, INPUT);
  pinMode(analogInputPinM, INPUT);
}

void loop()
{
  analogValueL = analogRead(sensorPinL);
  analogValueM = analogRead(sensorPinM);
  Serial.println(analogValueL);
  Serial.println(analogValueM);
  if (GPS.NMEAavailable()) {              // If a full NMEA sentence is recieved...
    GPSfix = GPS.readFix();                 // ...read the fix status.
    if (GPSfix) {                           // If there is a fix...
      GPStime = GPS.readTime();                // ...read the time/lat/long info from the module
      GPSlat = GPS.readLat();
      GPSlong = GPS.readLong();
      Serial.println("GPS fix:");             // Print out the results
      Serial.print("Time: ");
      Serial.println(GPStime);         // Time format (HHMMSS.SS)
      Serial.print("Lat: ");
      Serial.println(GPSlat, 5);       // Print out the Lat with 5 decimal places of info (DD.DDDDD)
      Serial.print("Long: ");
      Serial.println(GPSlong, 5);      // Print out the Lat with 5 decimal places of info (DDD.DDDDD
      
      myFile = SD.open("coords.csv", FILE_WRITE);
      myFile.print(GPStime);         // Time format (HHMMSS.SS)
      myFile.print(",");
      myFile.print(GPSlong, 5);      // Print out the Lat with 5 decimal places of info (DDD.DDDDD)
      myFile.print(",");
      myFile.print(GPSlat, 5);       // Print out the Lat with 5 decimal places of info (DD.DDDDD)
      myFile.print(",");

      if (analogValueL > 600 && analogValueM > 500){
        myFile.print("TAMPER DETECTED. Light/Motion indicated.");         // Tamper (yes/no)
        myFile.println();
      } else if (analogValueL > 600) {
        myFile.print("TAMPER DETECTED. Light indicated.");         // Tamper (yes/no)
        myFile.println();
      } else if (analogValueM > 500){
        myFile.print("TAMPER DETECTED. Motion indicated.");         // Tamper (yes/no)
        myFile.println();
        }
      else {
        myFile.print("");         // Tamper (yes/no)
        myFile.println();
      }
      myFile.close();
    } else {                                // ...else, there is no fix.
      //Serial.println("NO GPS fix!");
    }
  }
}

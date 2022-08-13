/**************************************************************************/
/*  
  Test code for using the Adafruit Ultimate GPS using Serial comms

  This code listens for full and available NMEA RMC sentences from the GPS 
  module. It also parses the sentences for latitude, longitude, time, and 
  fix status.

*/
/**************************************************************************/

#include <BuildMo_GPS.h>
#include <SoftwareSerial.h>

// If you are using hardware serial, make sure the two lines below are uncommented. (ARDUINO MEGA)
// #define GPSSerial Serial1
// BuildMo_GPS GPS(&GPSSerial);

// If you are using software serial, make sure the two lines below are uncommented. (ARDUINO UNO)
SoftwareSerial mySerial(3, 2);  // (Rx,Tx)
BuildMo_GPS GPS(&mySerial);

bool GPSfix;
float GPSlat, GPSlong, GPStime;


void setup()
{
  GPS.begin(9600);      // Starts Serial comms with GPS module and initializes the module
  Serial.begin(9600);   // Starts Serial comms with the computer
}

void loop() 
{
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
      Serial.println(GPSlong, 5);      // Print out the Lat with 5 decimal places of info (DDD.DDDDD) 
      Serial.println();
    } else {                                // ...else, there is no fix.
      Serial.println("NO GPS fix!");
    }
  }
}

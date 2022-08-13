/**************************************************************************/
/*
  @file BuildMo_GPS.h

  This is the Building Momentum GPS library. It is based on the Adafruit 
  ultimate GPS library with great simplifications to allow usage with 
  medium sized sketchs on the Arduino Uno.

  This library is intended for the Adafruit Ultimate GPS module
  with the MTK33x9 chipset (http://www.adafruit.com/products/746)
  
  Authors: A. Vega
  
  Version: v01
  
  Release date: 21 JUN 2021
  
  Changes from previous version: n/a
  
*/
/**************************************************************************/

#ifndef _BUILDMO_GPS_H
#define _BUILDMO_GPS_H

#define MAXLINELENGTH 			75 	// How long are max NMEA lines to parse?

#include "Arduino.h"
#include <SoftwareSerial.h>

#define PMTK_SET_NMEA_UPDATE_1HZ 				"$PMTK220,1000*1F"  // 1 Hz
#define PMTK_SET_NMEA_UPDATE_2HZ 				"$PMTK220,500*2B"   // 2 Hz

#define PMTK_SET_BAUD_9600 						"$PMTK251,9600*17"  // 9600 bps

#define PMTK_SET_NMEA_OUTPUT_RMCONLY			"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29" // Turn on only the GPRMC sentence

#define PMTK_ENABLE_SBAS						"$PMTK313,1*2E" // Enable SBAS satellite (only works with 1Hz output rate)
#define PMTK_ENABLE_WAAS 						"$PMTK301,2*2E" // Use WAAS for DGPS correction data

#define PGCMD_NOANTENNA 						"$PGCMD,33,0*6D" // don't show antenna status messages

class BuildMo_GPS : public Print {
public:
  BuildMo_GPS(SoftwareSerial *ser); // Constructor when using SoftwareSerial
  BuildMo_GPS(HardwareSerial *ser); // Constructor when using HardwareSerial
  void common_init(void);
  bool NMEAavailable(void);
  float readLat(void);
  float readLong(void);
  bool readFix(void);
  float readTime(void);
  
  char read(void);	
  size_t write(uint8_t);
  size_t available(void);
  bool begin(uint32_t);
  
  float latitude;  			// Latitude value in degr and min (DD.DDDD)
  float longitude;  		// Longitude value in degr and min (DDD.DDDD)
  bool fix;					// Variable for storing if the GPS module has a fix
  float time;				// Variable for storing the time (HHMMSS.SSS)
  char *sentence_buffer;	// Public pointer for accessing the NMEA sentence

private:

  SoftwareSerial *gpsSwSerial;
  HardwareSerial *gpsHwSerial;
  void NMEAread(void);
  byte buffer_index = 0;
  char checksum_accum = 0;
  bool sentence_avail = false;
  char incoming_buffer[MAXLINELENGTH];
  bool checksum_enable = false;
  byte checksum_index = 0;
  char checksum_buffer[3] = {'0', '0', '\0'};
};

#endif
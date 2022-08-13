/**************************************************************************/
/*
  @file BuildMo_GPS.cpp

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

#include <BuildMo_GPS.h>

BuildMo_GPS::BuildMo_GPS(SoftwareSerial *ser) {
  common_init();     // Set everything to common state, then...
  gpsSwSerial = ser; // ...override gpsSwSerial with value passed.
}

BuildMo_GPS::BuildMo_GPS(HardwareSerial *ser) {
  common_init();     // Set everything to common state, then...
  gpsHwSerial = ser; // ...override gpsHwSerial with value passed.
}

void BuildMo_GPS::common_init(void) {
  gpsSwSerial = NULL; // Set both to NULL, then override correct
  gpsHwSerial = NULL; // port pointer in corresponding constructor
}

// Function for determining if data is in the buffer from either the hardware or software serial port. 
// Returns the number of bytes available.
size_t BuildMo_GPS::available(void) {
  if (gpsSwSerial) {
    return gpsSwSerial->available();
  }
  if (gpsHwSerial) {
    return gpsHwSerial->available();
  }
  return 0;
}

// Function for writing to either the hardware or software serial port.
// Returns the number of bytes written.
size_t BuildMo_GPS::write(uint8_t c) {
  if (gpsSwSerial) {
    return gpsSwSerial->write(c);
  }
  if (gpsHwSerial) {
    return gpsHwSerial->write(c);
  }
  return 0;
}

// Function for reading a byte/char from either the hardware or software serial port.
// Returns the byte/char read.
char BuildMo_GPS::read(void) {
  char c = 0;
  
  if (gpsSwSerial) {
    if (!gpsSwSerial->available())
      return c;
    c = gpsSwSerial->read();
  }

  if (gpsHwSerial) {
    if (!gpsHwSerial->available())
      return c;
    c = gpsHwSerial->read();
  }
  return c;
}

// Function for staring either the wardware or software serial port.
// Also initializes the GPS module to output only RMC sentences at 1Hz.

bool BuildMo_GPS::begin(uint32_t baudrate) {

  if (gpsSwSerial) {
    gpsSwSerial->begin(baudrate);
  }
  if (gpsHwSerial) {
    gpsHwSerial->begin(baudrate);
  }
  
  delay(100);
  
  println(PMTK_SET_NMEA_UPDATE_1HZ);
  println(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  
  return true;
}

// Function for determining if an NMEA sentence is available.
// Each time this function is called, it determines if serial data is availble, 
// organizes it into an incoming buffer, and calculates the checksum as data arrives.
bool BuildMo_GPS::NMEAavailable(void){
	if (available()){
		char incoming_char;
		while (available()){
			incoming_char = read();
			if (incoming_char == '$'){
				buffer_index = 0;
				checksum_accum = 0;
				checksum_enable = true;
			}else if (incoming_char == '*'){
				checksum_enable = false;
				checksum_index = 0;
			}
			incoming_buffer[buffer_index++]=incoming_char;
			if (checksum_enable && (incoming_char != '$')){
				checksum_accum = checksum_accum ^ incoming_char;
			}
			if (((!checksum_enable) && (incoming_char != '*')) && ((checksum_index == 0) || (checksum_index == 1))){
				checksum_buffer[checksum_index++] = incoming_char;
			}
			if (incoming_char == 0x0A){
				char temp_char = (checksum_buffer[0] & 0x0F) + ((checksum_buffer[0] & 0x40) >> 3) + ((checksum_buffer[0] & 0x40) >> 6) << 4;
				temp_char += (checksum_buffer[1] & 0x0F) + ((checksum_buffer[1] & 0x40) >> 3) + ((checksum_buffer[1] & 0x40) >> 6);
				if(temp_char == checksum_accum){
					sentence_avail = true;
					sentence_buffer = incoming_buffer;
					NMEAread();
				}
			}
		}
	}
	return sentence_avail;
}


void BuildMo_GPS::NMEAread(void){
	char time_char[9], lat_char[10], long_char[11];
    int i = 0, j;
	
	for (j = 7; j < 15; j++) {     
		time_char[i++] = incoming_buffer[j];   // ...take the TIME info and store it into another string
	}
	time = atof(time_char);    // Convert the string into a floating point variable
	i = 0;
	
	if (incoming_buffer[18] == 'A'){
		fix = true;
		j=20;
		while (incoming_buffer[j++] != ',') {
			lat_char[i++] = incoming_buffer[j-1];  // ...take the LONG info and store it into another string
		}
		float gps_lat_temp = atof(lat_char);    // Convert the string into a floating point variable
		latitude = gps_lat_temp / 60 - floor(gps_lat_temp / 100) * 2 / 3;    // Convert from deg mm.mmm format to deg only 
		if (incoming_buffer[j++] == 'S') {       // If in the southern hemisphere, negate the value
			latitude = -latitude;
		}
		i = 0;
		j++;
		while (incoming_buffer[j++] != ',') {
			long_char[i++] = incoming_buffer[j-1];  // ...take the LONG info and store it into another string
		}
		float gps_long_temp = atof(long_char);  // Convert the string into a floating point variable
		longitude = gps_long_temp / 60 - floor(gps_long_temp / 100) * 2 / 3;   // Convert from deg mm.mmm format to deg only
		if (incoming_buffer[j++] == 'W') {       // If in the western hemisphere, negate the value
			longitude = -longitude;
		}
	}else{
		fix = false;
		latitude = 0;
		longitude = 0;
	}
	return;
}

// Function for returning the time of the GPS module.
float BuildMo_GPS::readTime(void){
	sentence_avail = false;
	return time;    
}

// Function for returning the latitude of the GPS module.
float BuildMo_GPS::readLat(void){
	sentence_avail = false;
	return latitude;
}

// Function for returning the longitude of the GPS module.
float BuildMo_GPS::readLong(void){
	sentence_avail = false;
	return longitude;
}

// Function for returning the fix status of the GPS module.
bool BuildMo_GPS::readFix(void){
	return fix;
}

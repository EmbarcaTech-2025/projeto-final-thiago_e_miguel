#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"
#include <string.h>

#define GPS_UART_ID uart0
#define GPS_BAUD_RATE 9600
#define GPS_UART_TX_PIN 0
#define GPS_UART_RX_PIN 1
#define MAX_NMEA_SENTENCE_LENGTH 82

#define GPS_TURN_OFF_ANTENNA_UPDATE_CMD "$PGCMD,33,0*6D\r\n"
#define PMTK_SET_NMEA_UPDATE_1HZ "$PMTK220,1000*1F" ///<  1 Hz
#define PMTK_SET_NMEA_UPDATE_2HZ "$PMTK220,500*2B"  ///<  2 Hz
#define PMTK_SET_NMEA_UPDATE_5HZ "$PMTK220,200*2C"  ///<  5 Hz
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F" ///< 10 Hz
#define PMTK_SET_NMEA_OUTPUT_RMCGGA                                            \
  "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

class GPS_GYNEO6MV2 {
  public:
    GPS_GYNEO6MV2();
    void begin();

    char* getLastNMEA();
    bool hasNewNMEA();

    void update();
    
    float getSpeedKPH();
    float getLatitude();
    float getLongitude();
    float getAltitude();
    int getSatellites();
  private:
    void parseNMEA();
    inline bool sendCommand(const char* cmd) {
      if (!uart_is_writable(GPS_UART_ID)) return false;
      uart_write_blocking(GPS_UART_ID, (const uint8_t*)cmd, strlen(cmd));
      return true;
    }
    bool validateNMEAChecksum(const char* nmea);
    double nmeaCoordToDecimal(const char *token);

    float latitude;
    float longitude;
    float altitude;
    int satellites;
    float speed_kph;
    
    char last_NMEA[MAX_NMEA_SENTENCE_LENGTH];
    bool new_nmea;
    char c;
    char current_NMEA[MAX_NMEA_SENTENCE_LENGTH];
};

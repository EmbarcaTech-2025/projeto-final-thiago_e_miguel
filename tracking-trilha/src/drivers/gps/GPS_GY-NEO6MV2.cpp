#include "GPS_GY-NEO6MV2.h"
#include <string.h>
#include <math.h>   // floor, fabs
#include <cctype>   // isxdigit, toupper

GPS_GYNEO6MV2::GPS_GYNEO6MV2() {
  latitude = 0.0;
  longitude = 0.0;
  altitude = 0.0;
  satellites = 0;
  speed_kph = 0.0;
  last_NMEA[0] = '\0';
  new_nmea = false;
  c = '\0';
  current_NMEA[0] = '\0';
}

bool GPS_GYNEO6MV2::validateNMEAChecksum(const char *nmea) {
  // const char* nmea é a linha completa começando com '$' e possivelmente contendo '*XX' no final
  const char *p = nmea;
  if (!p || p[0] != '$') return false;
  uint8_t cs = 0;
  p++; // p aponta para primeiro char após $
  while (*p && *p != '*') {
    cs ^= (uint8_t)*p;
    p++;
  }
  if (*p != '*') return false; // sem checksum
  p++;
  // parse hex dois dígitos
  int hi = -1, lo = -1;
  if (isxdigit((unsigned char)p[0])) hi = (p[0] <= '9' ? p[0]-'0' : (toupper(p[0])-'A'+10));
  if (isxdigit((unsigned char)p[1])) lo = (p[1] <= '9' ? p[1]-'0' : (toupper(p[1])-'A'+10));
  if (hi < 0 || lo < 0) return false;
  uint8_t expected = (hi << 4) | lo;
  return cs == expected;
}

double GPS_GYNEO6MV2::nmeaCoordToDecimal(const char *token) {
  // token exemplo: "4916.45" (ddmm.mmmm) ou "12319.1234" (dddmm.mmmm)
  if (!token || *token == '\0') return 0.0;
  double v = atof(token);
  double deg = floor(v / 100.0);         // graus
  double minutes = v - deg * 100.0;      // minutos
  return deg + (minutes / 60.0);
}

void GPS_GYNEO6MV2::begin() {
  uart_init(GPS_UART_ID, GPS_BAUD_RATE);
  uart_set_translate_crlf(GPS_UART_ID, false);
  
  gpio_set_function(GPS_UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(GPS_UART_RX_PIN, GPIO_FUNC_UART);

  // Declaração para o pico explorer / tooling
  bi_decl(bi_1pin_with_func(GPS_UART_TX_PIN, GPIO_FUNC_UART));
  bi_decl(bi_1pin_with_func(GPS_UART_RX_PIN, GPIO_FUNC_UART));

  sendCommand(GPS_TURN_OFF_ANTENNA_UPDATE_CMD);
  sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
  sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
}

bool GPS_GYNEO6MV2::hasNewNMEA() {
  return new_nmea;
}

char* GPS_GYNEO6MV2::getLastNMEA() {
  new_nmea = false;
  return last_NMEA;
}

void GPS_GYNEO6MV2::update() {
  while (uart_is_readable(GPS_UART_ID)) {
    c = uart_getc(GPS_UART_ID);
    printf("c: %c\n", c);
    if (c == '\n') {
      current_NMEA[strcspn(current_NMEA, "\r\n")] = '\0'; // Remove any trailing newline or carriage return
      strncpy(last_NMEA, current_NMEA, MAX_NMEA_SENTENCE_LENGTH);
      last_NMEA[MAX_NMEA_SENTENCE_LENGTH - 1] = '\0'; // Ensure null-termination
      new_nmea = true;
      parseNMEA();
      current_NMEA[0] = '\0'; // Reset for next sentence
    } else {
      size_t len = strlen(current_NMEA);
      if (len < MAX_NMEA_SENTENCE_LENGTH - 1) {
        current_NMEA[len] = c;
        current_NMEA[len + 1] = '\0';
      } else {
        current_NMEA[MAX_NMEA_SENTENCE_LENGTH - 1] = '\0'; // Ensure null-termination
      }
    }
  }
}

// parseNMEA() substituto — usa verificação de checksum e parse seguro
void GPS_GYNEO6MV2::parseNMEA() {
  // trabalhar em cópia para não modificar last_NMEA
  char buf[MAX_NMEA_SENTENCE_LENGTH];
  strncpy(buf, last_NMEA, sizeof(buf));
  buf[sizeof(buf)-1] = '\0';

  if (!validateNMEAChecksum(buf)) {
    return; // linha inválida
  }

  // tokenização segura com strtok_r
  char *saveptr = nullptr;
  char *token = strtok_r(buf, ",", &saveptr);
  if (!token) return;

  if (strcmp(token, "$GPRMC") == 0 || strcmp(token, "$GNRMC") == 0) {
    // $GPRMC,hhmmss.sss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,...
    // campo1 = time
    token = strtok_r(nullptr, ",", &saveptr); // tempo
    token = strtok_r(nullptr, ",", &saveptr); // status
    if (!token || token[0] != 'A') return; // inválido

    // latitude
    char *lat_tok = strtok_r(nullptr, ",", &saveptr);
    char *ns_tok  = strtok_r(nullptr, ",", &saveptr);
    if (lat_tok && ns_tok) {
      double lat = nmeaCoordToDecimal(lat_tok);
      if (ns_tok[0] == 'S') lat = -lat;
      latitude = (float)lat;
    }

    // longitude
    char *lon_tok = strtok_r(nullptr, ",", &saveptr);
    char *ew_tok  = strtok_r(nullptr, ",", &saveptr);
    if (lon_tok && ew_tok) {
      double lon = nmeaCoordToDecimal(lon_tok);
      if (ew_tok[0] == 'W') lon = -lon;
      longitude = (float)lon;
    }

    // speed (knots -> kph)
    char *spd_tok = strtok_r(nullptr, ",", &saveptr);
    if (spd_tok) {
      double spd_knots = atof(spd_tok);
      speed_kph = (float)(spd_knots * 1.852);
    }
    // resto não necessário aqui
  }
  else if (strcmp(token, "$GPGGA") == 0 || strcmp(token, "$GNGGA") == 0) {
    // $GPGGA,hhmmss.sss,llll.ll,a,yyyyy.yy,a,quality,num_sats,hdop,altitude,units,...
    strtok_r(nullptr, ",", &saveptr); // time
    char *lat_tok = strtok_r(nullptr, ",", &saveptr);
    char *ns_tok  = strtok_r(nullptr, ",", &saveptr);
    char *lon_tok = strtok_r(nullptr, ",", &saveptr);
    char *ew_tok  = strtok_r(nullptr, ",", &saveptr);
    char *quality = strtok_r(nullptr, ",", &saveptr);
    char *sat_tok = strtok_r(nullptr, ",", &saveptr);

    if (lat_tok && ns_tok) {
      double lat = nmeaCoordToDecimal(lat_tok);
      if (ns_tok[0] == 'S') lat = -lat;
      latitude = (float)lat;
    }
    if (lon_tok && ew_tok) {
      double lon = nmeaCoordToDecimal(lon_tok);
      if (ew_tok[0] == 'W') lon = -lon;
      longitude = (float)lon;
    }
    if (sat_tok) satellites = atoi(sat_tok);

    // pular hdop
    char *alt_tok = strtok_r(nullptr, ",", &saveptr);
    if (alt_tok) altitude = atof(alt_tok);
  }
}


float GPS_GYNEO6MV2::getSpeedKPH() {
  return speed_kph;
}

float GPS_GYNEO6MV2::getLatitude() {
  return latitude;
}

float GPS_GYNEO6MV2::getLongitude() {
  return longitude;
}

float GPS_GYNEO6MV2::getAltitude() {
  return altitude;
}

int GPS_GYNEO6MV2::getSatellites() {
  return satellites;
}

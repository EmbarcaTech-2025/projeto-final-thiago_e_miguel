#include "gps.h"

#ifdef USE_GPS
GPS::GPS() : Sensor(), gpsModule() {
    buffer_size_lat = 0;
    buffer_size_lon = 0;
    buffer_size_alt = 0;
    buffer_size_sat = 0;
    buffer_size_spd = 0;
    gpsModule.begin();
}

void GPS::Update() {
    printf("Updating GPS\n");
    gpsModule.update();
    if (
      gpsModule.hasNewNMEA() &&
      buffer_size_alt < MAX_BUFFER_SIZE &&
      buffer_size_lat < MAX_BUFFER_SIZE &&
      buffer_size_lon < MAX_BUFFER_SIZE &&
      buffer_size_sat < MAX_BUFFER_SIZE &&
      buffer_size_spd < MAX_BUFFER_SIZE
    ) {
        printf("New NMEA\n");
        float lat = gpsModule.getLatitude();
        float lon = gpsModule.getLongitude();
        float alt = gpsModule.getAltitude();
        int sat = gpsModule.getSatellites();
        float spd = gpsModule.getSpeedKPH();

        if (lat != 0.0f) {
            buffer_lat[buffer_size_lat++] = lat;
        }
        if (lon != 0.0f) {
            buffer_lon[buffer_size_lon++] = lon;
        }
        if (alt != 0.0f) {
            buffer_alt[buffer_size_alt++] = alt;
        }
        if (sat != 0) {
            buffer_sat[buffer_size_sat++] = static_cast<float>(sat);
        }
        if (spd != 0.0f) {
            buffer_spd[buffer_size_spd++] = spd;
        }
    } else {
        printf("No new NMEA\n");
    }
}

bool GPS::setDataWithBuffer(Data_t* data, float* buffer, size_t* buffer_size) {
  if (*buffer_size == 0) {
      return false;
  }

  data->data = buffer;
  data->size = *buffer_size;
  *buffer_size = 0;
  data->timestamp = to_ms_since_boot(get_absolute_time());
  return true;
}

bool GPS::getData(Data_t* data) {    
    switch (data->type) {
    case SAMPLE_TYPE_LATITUDE:
      return setDataWithBuffer(data, buffer_lat, &buffer_size_lat);
      break;
    case SAMPLE_TYPE_LONGITUDE:
      return setDataWithBuffer(data, buffer_lon, &buffer_size_lon);
      break;
    case SAMPLE_TYPE_ALTITUDE:
      return setDataWithBuffer(data, buffer_alt, &buffer_size_alt);
      break;
    case SAMPLE_TYPE_SATELLITES:
      return setDataWithBuffer(data, buffer_sat, &buffer_size_sat);
      break;
    case SAMPLE_TYPE_SPEED_KPH:
      return setDataWithBuffer(data, buffer_spd, &buffer_size_spd);
      break;
    default:
      return false;
  }
}
#endif

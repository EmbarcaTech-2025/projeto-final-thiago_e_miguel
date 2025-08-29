#include "oximeter.h"
#include "utils.h"

MAX3010X heartSensor(I2C_PORT_OXI, PIN_WIRE_SDA_OXI, PIN_WIRE_SCL_OXI, I2C_SPEED_FAST);

Oximeter::Oximeter() : Sensor() {
  busy_wait_ms(500);
	while (heartSensor.begin() != true) {
		printf("MAX30102 not connect r fail load calib coeff \r\n");
		busy_wait_ms(500);
	}

  sensorType = SENSOR_TYPE_OXIMETER;

	uint8_t powerLevel = 0x1f; //Options: 0=Off to 255=50mA
	uint8_t sampleAverage = 0x04; //Options: 1, 2, 4, 8, 16, 32
	uint8_t ledMode = 0x03; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
	int sampleRate = 400; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
	int pulseWidth = 411; //Options: 69, 118, 215, 411
	int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
	heartSensor.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

bool Oximeter::getData(Data_t* data) {
  if (!is_valid()) {
    return false;
  }

  switch (data->type) {
    case SAMPLE_TYPE_SPO2:
      if (buffer_size_spO2 == 0) {
        return false;
      }
      data->data = buffer_spO2;
      data->size = buffer_size_spO2;
      buffer_size_spO2 = 0;
      break;
    case SAMPLE_TYPE_HEART_RATE:
      if (buffer_size_heart_rate == 0) {
        return false;
      }
      data->data = buffer_heart_rate;
      data->size = buffer_size_heart_rate;
      buffer_size_heart_rate = 0;
      break;
    case SAMPLE_TYPE_TEMPERATURE:
      if (buffer_size_temperature == 0) {
        return false;
      }
      data->data = buffer_temperature;
      data->size = buffer_size_temperature;
      buffer_size_temperature = 0;
      break;
    default:
      return false;
  }

  data->timestamp = to_ms_since_boot(get_absolute_time());

  return true;
}

void Oximeter::Update() {
  uint32_t aun_ir_buffer[BUFFER_SIZE_ALGORITHM]; //infrared LED sensor data
  uint32_t aun_red_buffer[BUFFER_SIZE_ALGORITHM];  //red LED sensor data
  
  for(int i=0;i<BUFFER_SIZE_ALGORITHM;i++) { //store the samples in the memory
    aun_red_buffer[i]=heartSensor.getRed();
    aun_ir_buffer[i]=heartSensor.getIR();
  }

  float ratio,correl;
  rf_heart_rate_and_oxygen_saturation(
    aun_ir_buffer,
    BUFFER_SIZE_ALGORITHM,
    aun_red_buffer,
    &n_spo2,
    &ch_spo2_valid,
    &n_heart_rate,
    &ch_hr_valid,
    &ratio,
    &correl
  ); 
  //maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

  float temperature = heartSensor.readTemperature();

  if (is_valid()) {
    if (buffer_size_spO2 >= MAX_BUFFER_SIZE) {
      shift_buffer(buffer_spO2, &buffer_size_spO2);
    }
    if (buffer_size_heart_rate >= MAX_BUFFER_SIZE) {
      shift_buffer(buffer_heart_rate, &buffer_size_heart_rate);
    }
    if (buffer_size_temperature >= MAX_BUFFER_SIZE) {
      shift_buffer(buffer_temperature, &buffer_size_temperature);
    }

    buffer_spO2[buffer_size_spO2++] = n_spo2;
    buffer_heart_rate[buffer_size_heart_rate++] = n_heart_rate;
    buffer_temperature[buffer_size_temperature++] = temperature;
  }
}

bool Oximeter::is_valid() {
  return (ch_hr_valid && ch_spo2_valid);
}


#include "oximeter.h"
#include "utils.h"
#include "hardware/clocks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "../../include/utils/debug_printf.h"

MAX3010X heartSensor(I2C_PORT_OXI, PIN_WIRE_SDA_OXI, PIN_WIRE_SCL_OXI, I2C_SPEED_FAST);

Oximeter::Oximeter() : Sensor() {
  busy_wait_ms(500);
	while (heartSensor.begin() != true) {
		PRINTF_DEBUG("MAX30102 not connect r fail load calib coeff \r\n");
		busy_wait_ms(500);
	}

  sensorType = SENSOR_TYPE_OXIMETER;

	uint8_t powerLevel = 0x1f; //Options: 0=Off to 255=50mA
	uint8_t sampleAverage = 0x04; //Options: 1, 2, 4, 8, 16, 32
	uint8_t ledMode = 0x03; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
	int sampleRate = 1600; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
	int pulseWidth = 411; //Options: 69, 118, 215, 411
	int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
	heartSensor.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
	
	// Initialize validation flags
	ch_spo2_valid = 0;
	ch_hr_valid = 0;
	
	// Initialize FreeRTOS components
	taskHandle = nullptr;
	dataMutex = xSemaphoreCreateMutex();
	taskRunning = false;
}

Oximeter::~Oximeter() {
	StopTask();
	if (dataMutex != nullptr) {
		vSemaphoreDelete(dataMutex);
	}
}

bool Oximeter::getData(Data_t* data) {
  if (!is_valid()) {
    return false;
  }

  // Take mutex to safely access shared data
  if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
    bool result = false;
    
    switch (data->type) {
      case SAMPLE_TYPE_SPO2:
        if (buffer_size_spO2 == 0) {
          PRINTF_DEBUG("No SPO2 data available\n");
          result = false;
        } else {
          PRINTF_DEBUG("Consuming %d SPO2 samples\n", buffer_size_spO2);
          data->data = buffer_spO2;
          data->size = buffer_size_spO2;
          buffer_size_spO2 = 0;
          result = true;
        }
        break;
      case SAMPLE_TYPE_HEART_RATE:
        if (buffer_size_heart_rate == 0) {
          PRINTF_DEBUG("No HEART_RATE data available\n");
          result = false;
        } else {
          PRINTF_DEBUG("Consuming %d HEART_RATE samples\n", buffer_size_heart_rate);
          data->data = buffer_heart_rate;
          data->size = buffer_size_heart_rate;
          buffer_size_heart_rate = 0;
          result = true;
        }
        break;
      case SAMPLE_TYPE_TEMPERATURE:
        if (buffer_size_temperature == 0) {
          PRINTF_DEBUG("No TEMPERATURE data available\n");
          result = false;
        } else {
          PRINTF_DEBUG("Consuming %d TEMPERATURE samples\n", buffer_size_temperature);
          data->data = buffer_temperature;
          data->size = buffer_size_temperature;
          buffer_size_temperature = 0;
          result = true;
        }
        break;
      default:
        result = false;
    }

    data->timestamp = to_ms_since_boot(get_absolute_time());
    
    xSemaphoreGive(dataMutex);
    return result;
  }
  
  return false;
}

void Oximeter::Update() {
  // This method is now deprecated - use StartTask() instead
  // For backward compatibility, call UpdateInternal directly
  // UpdateInternal();
}

void Oximeter::UpdateInternal() {
  uint32_t aun_ir_buffer[BUFFER_SIZE_ALGORITHM]; //infrared LED sensor data
  uint32_t aun_red_buffer[BUFFER_SIZE_ALGORITHM];  //red LED sensor data

  PRINTF_DEBUG("Oximeter Time: %llu\n", get_absolute_time());
  
  // Collect samples with minimal delay - use FIFO data when available
  for(int i=0;i<BUFFER_SIZE_ALGORITHM;i++) { //store the samples in the memory
    // Check if new data is available in FIFO
    if(heartSensor.available() > 0) {
      aun_red_buffer[i] = heartSensor.getFIFORed();
      aun_ir_buffer[i] = heartSensor.getFIFOIR();
      heartSensor.nextSample();
    } else {
      // Fallback to immediate readings with shorter timeout
      aun_red_buffer[i] = heartSensor.getRed();
      aun_ir_buffer[i] = heartSensor.getIR();
    }
    // Small delay to allow sensor to collect new samples
    //vTaskDelay(pdMS_TO_TICKS(1));
    busy_wait_us(625); // 625us = 1/1600Hz for 1600Hz sample rate
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
    // Take mutex to safely update shared data
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
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
      
      xSemaphoreGive(dataMutex);
    }
  }
}

bool Oximeter::is_valid() {
  return (ch_hr_valid && ch_spo2_valid);
}

void Oximeter::StartTask() {
  if (taskHandle == nullptr && dataMutex != nullptr) {
    taskRunning = true;
    BaseType_t result = xTaskCreate(
      OximeterTask,
      "OximeterTask",
      OXIMETER_TASK_STACK_SIZE,
      this,
      OXIMETER_TASK_PRIORITY,
      &taskHandle
    );
    
    if (result != pdPASS) {
      PRINTF_DEBUG("Failed to create Oximeter task\n");
      taskRunning = false;
      taskHandle = nullptr;
    } else {
      PRINTF_DEBUG("Oximeter task created successfully\n");
    }
  }
}

void Oximeter::StopTask() {
  if (taskHandle != nullptr) {
    taskRunning = false;
    vTaskDelete(taskHandle);
    taskHandle = nullptr;
    PRINTF_DEBUG("Oximeter task stopped\n");
  }
}

void Oximeter::OximeterTask(void* pvParameters) {
  Oximeter* oximeter = static_cast<Oximeter*>(pvParameters);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  
  PRINTF_DEBUG("Oximeter task started\n");
  
  while (oximeter->taskRunning) {
    PRINTF_DEBUG("Oximeter task time: %llu\n", get_absolute_time());
    
    // Check stack usage
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(nullptr);
    PRINTF_DEBUG("Oximeter task stack high water mark: %d\n", stackHighWaterMark);
    
    if (stackHighWaterMark < 100) {
        PRINTF_DEBUG("WARNING: Oximeter task stack is running low!\n");
    }
    
    oximeter->UpdateInternal();
    
    // Wait for the next period
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(OXIMETER_UPDATE_PERIOD_MS));
  }
  
  PRINTF_DEBUG("Oximeter task ending\n");
  vTaskDelete(nullptr);
}


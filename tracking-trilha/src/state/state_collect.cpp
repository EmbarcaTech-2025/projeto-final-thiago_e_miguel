#include "state_collect.h"
#include "oximeter.h"
#include "accelerometer.h"


// insert here all wanted_samples
sample_t StateCollect::wanted_samples[] = {
    SAMPLE_TYPE_SPO2,
    SAMPLE_TYPE_HEART_RATE,
    SAMPLE_TYPE_TEMPERATURE,
    SAMPLE_TYPE_ACCEL_X,
    SAMPLE_TYPE_ACCEL_Y,
    SAMPLE_TYPE_ACCEL_Z
};

Oled* StateCollect::oled = nullptr;

StateCollect::StateCollect() : State() {
   // Initialize FreeRTOS components
   taskHandle = nullptr;
   taskRunning = false;
}

StateCollect::~StateCollect() {
   StopTask();
}

void StateCollect::PrintOled(int line_index, const char* text) {
    if (oled != nullptr) {
        oled->PrintText(line_index, text);
    }
}

void StateCollect::Update() {
    // This method is now deprecated - use StartTask() instead
    // For backward compatibility, call UpdateInternal directly
    UpdateInternal();
}

void StateCollect::UpdateInternal() {
    PrintOled(0, "Coletando...     ");

    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        if (sensorArray[i] != nullptr) {
            // Skip oximeter update as it runs in its own task
            if (i != SENSOR_TYPE_OXIMETER) {
                sensorArray[i]->Update();
            }
        }
    }

    // Print wanted_samples
    for (size_t sensor_type = 0; sensor_type < SENSOR_TYPE_QTT; sensor_type++) {
        Sensor* sensor = GetSensor((sensor_t)sensor_type);
        if (sensor != nullptr) {
            Data_t data;
            for (size_t sample_index = 0; sample_index < SAMPLE_TYPE_QTT; sample_index++) {
              Analyzer* analyzer = GetAnalyzer((sensor_t)sensor_type, StateCollect::wanted_samples[sample_index]);
              data.type = StateCollect::wanted_samples[sample_index];
              if (sensor->getData(&data)) {
                  char data_str[17];
                  sprintf(data_str, "S%2d T%2d V%2.1f", sensor_type, data.type, data.data[0]);
                  PrintOled(sample_index + 1, data_str);
                  printf("Sensor Type: %d, Sample Type: %d, Data: ", sensor_type, data.type);
                  for (size_t buffer_index = 0; buffer_index < data.size; buffer_index++) {
                      printf("%.3f ", data.data[buffer_index]);
                  }
                  if (analyzer != nullptr) {
                      healthStatus_t healthStatus = analyzer->Analyze(&data);
                      char health_status_str[17];
                      sprintf(health_status_str, "H%2d", healthStatus);
                      PrintOled(7, health_status_str);
                      printf("Health Status: %d\n", healthStatus);
                  } else {
                    printf("No analyzer found for sensor type: %d\n", sensor_type);
                  }
                  printf("\n");
              }
          }
        }
    }

    if (oled != nullptr) {
        oled->Render();
    }
}

void StateCollect::Pause() {
    // Pause implementation
}

void StateCollect::Resume() {
    // Resume implementation
}

void StateCollect::StartTask() {
    if (taskHandle == nullptr) {
        taskRunning = true;
        BaseType_t result = xTaskCreate(
            StateTask,
            "StateTask",
            STATE_TASK_STACK_SIZE,
            this,
            STATE_TASK_PRIORITY,
            &taskHandle
        );
        
        if (result != pdPASS) {
            printf("Failed to create State task\n");
            taskRunning = false;
            taskHandle = nullptr;
        } else {
            printf("State task created successfully\n");
        }
    }
}

void StateCollect::StopTask() {
    if (taskHandle != nullptr) {
        taskRunning = false;
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
        printf("State task stopped\n");
    }
}

void StateCollect::StateTask(void* pvParameters) {
    StateCollect* stateCollect = static_cast<StateCollect*>(pvParameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    printf("State task started\n");
    
    while (stateCollect->taskRunning) {
        stateCollect->UpdateInternal();
        
        // Wait for the next period
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(STATE_UPDATE_PERIOD_MS));
    }
    
    printf("State task ending\n");
    vTaskDelete(nullptr);
}

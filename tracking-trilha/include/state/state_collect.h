#pragma once

#include "state.h"
#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "SD.h"
#include "buzzer.h"

// FreeRTOS task configuration for StateCollect
#define STATE_TASK_PRIORITY (configMAX_PRIORITIES - 2)
#define STATE_TASK_STACK_SIZE 4096  // Increased from 2048 to prevent stack overflow
#define STATE_UPDATE_PERIOD_MS 100  // Update every 100ms

#define SAMPLES_FILENAME_SIZE 100
#define SAMPLES_FILE_NAME "samples.csv"
#define SAMPLES_FILE_HEADER "sensor_type,sample_type,timestamp,data,health_status\n"

class StateCollect : public State {
public:
    StateCollect();
    ~StateCollect();

    virtual void Update() override;
    virtual void Pause() override;
    virtual void Resume() override;

    inline void setOled(Oled* oledInstance) { oled = oledInstance; }
    inline void setSD(SD* sdInstance) { sd = sdInstance; }
    inline void setBuzzer(Buzzer* buzzerInstance) { buzzer = buzzerInstance; }
    inline void setSamplesFilename(const char* filename) { strncpy(samples_filename, filename, SAMPLES_FILENAME_SIZE); }

    // Task management methods
    void StartTask();
    void StopTask();
    
private:
  static sample_t wanted_samples[SAMPLE_TYPE_QTT];
  static Oled *oled;
  static SD *sd;
  static Buzzer *buzzer;
  static char samples_filename[SAMPLES_FILENAME_SIZE];

  void PrintOled(int line_index, const char* text);
  void UpdateInternal();
  static void StateTask(void* pvParameters);
  
  // FreeRTOS task management
  TaskHandle_t taskHandle;
  bool taskRunning;
};

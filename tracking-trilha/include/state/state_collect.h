#pragma once

#include "state.h"
#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// FreeRTOS task configuration for StateCollect
#define STATE_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define STATE_TASK_STACK_SIZE 2048
#define STATE_UPDATE_PERIOD_MS 100  // Update every 100ms

class StateCollect : public State {
public:
    StateCollect();
    ~StateCollect();

    virtual void Update() override;
    virtual void Pause() override;
    virtual void Resume() override;

    inline void setOled(Oled* oledInstance) { oled = oledInstance; }
    
    // Task management methods
    void StartTask();
    void StopTask();
    
private:
  static sample_t wanted_samples[SAMPLE_TYPE_QTT];
  static Oled *oled;

  void PrintOled(int line_index, const char* text);
  void UpdateInternal();
  static void StateTask(void* pvParameters);
  
  // FreeRTOS task management
  TaskHandle_t taskHandle;
  bool taskRunning;
};

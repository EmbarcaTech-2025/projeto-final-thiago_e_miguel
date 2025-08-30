#pragma once

#include "state.h"
#include "oled.h"

class StateCollect : public State {
public:
    StateCollect();

    virtual void Update() override;
    virtual void Pause() override;
    virtual void Resume() override;

    inline void setOled(Oled* oledInstance) { oled = oledInstance; }
private:
  static sample_t wanted_samples[SAMPLE_TYPE_QTT];
  static Oled *oled;

  void PrintOled(int line_index, const char* text);
};

#pragma once

#include "state.h"

class StateCollect : public State {
public:
    StateCollect();

    virtual void Update() override;
    virtual void Pause() override;
    virtual void Resume() override;
private:
  static sample_t wanted_samples[SAMPLE_TYPE_QTT];
};

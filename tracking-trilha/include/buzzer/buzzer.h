// buzzer.h
#pragma once
#include "pico/stdlib.h"

class Buzzer {
public:
  Buzzer();
  // freq_hz = 0 -> desliga
  void setTone(uint32_t freq_hz, uint8_t duty_percent = 50);
  void stop(); // para o som de vez
  bool isPlaying() const;

private:
  bool playing = false;
  uint slice_num = 0;
  uint gpio = 0;
};

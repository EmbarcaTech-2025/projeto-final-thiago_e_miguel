// buzzer.cpp
#include "buzzer.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#ifndef BUZZER_PIN
#define BUZZER_PIN 21
#endif

Buzzer::Buzzer() {
  gpio = BUZZER_PIN;
  gpio_set_function(gpio, GPIO_FUNC_SIO); // começa como SIO
  gpio_set_dir(gpio, GPIO_OUT);
  gpio_put(gpio, 0);
  slice_num = pwm_gpio_to_slice_num(gpio);
  playing = false;
}

void Buzzer::setTone(uint32_t freq_hz, uint8_t duty_percent) {
  if (freq_hz == 0 || duty_percent == 0) {
    stop();
    return;
  }
  if (duty_percent > 100) duty_percent = 100;

  const uint64_t clk = clock_get_hz(clk_sys);
  // estratégia: usar wrap razoável (1000) e ajustar clkdiv; se precisar aumentamos wrap
  uint32_t wrap = 1000 - 1; // 1000 passos
  float clkdiv = (float)clk / ((float)(wrap + 1) * (float)freq_hz);

  if (clkdiv < 1.0f) clkdiv = 1.0f;
  if (clkdiv > 255.0f) {
    // se clkdiv fica >255, recalcula wrap pra caber (limita clkdiv a 255)
    clkdiv = 255.0f;
    float tmp = (float)clk / (clkdiv * (float)freq_hz);
    if (tmp < 2.0f) tmp = 2.0f;
    if (tmp > 0xFFFF) tmp = 0xFFFF;
    wrap = (uint32_t)tmp - 1;
  }

  // segura limites
  if (wrap == 0) wrap = 1;
  if (wrap > 0xFFFFu) wrap = 0xFFFFu;

  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, clkdiv);
  pwm_config_set_wrap(&config, wrap);

  // ativa PWM no pino
  gpio_set_function(gpio, GPIO_FUNC_PWM);
  pwm_init(slice_num, &config, true);

  // duty -> level
  uint32_t level = ((uint64_t)(wrap + 1) * duty_percent) / 100u;
  if (level > wrap) level = wrap;
  pwm_set_gpio_level(gpio, level);
  pwm_set_enabled(slice_num, true);

  playing = true;
}

void Buzzer::stop() {
  // desabilita e garante pino em 0
  pwm_set_enabled(slice_num, false);
  // garante que a função do pino não fique controlada pelo PWM
  gpio_set_function(gpio, GPIO_FUNC_SIO);
  gpio_set_dir(gpio, GPIO_OUT);
  gpio_put(gpio, 0);
  playing = false;
}

bool Buzzer::isPlaying() const {
  return playing;
}

#include "oled.h"

Oled::Oled() {
  init_OLed();
}

void Oled::Clear() {
  clear_OLed();
}

void Oled::DrawPixel(int x, int y, bool is_white) {
  draw_point_OLed(x, y, is_white);
}

void Oled::DrawLine(int x0, int y0, int x1, int y1, bool is_white) {
  draw_line_OLed(x0, y0, x1, y1, is_white);
}

void Oled::DrawRect(int x0, int y0, int x1, int y1, bool is_white) {
  draw_rect_OLed(x0, y0, x1, y1, is_white);
}

void Oled::PrintText(int line_index, const char* text) {
  int y = line_index * CHAR_HEIGHT;
  char* o_led_text[] = {
    (char*)text
  };
  print_lines_OLed(o_led_text, 1, 0, y);
}

void Oled::Render() {
  render_OLed();
}

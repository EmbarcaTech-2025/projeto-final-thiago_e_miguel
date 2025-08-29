#pragma once

#include "display_oled.h"

class Oled {
public:
    Oled();
    void Clear();
    void DrawPixel(int x, int y, bool is_white);
    void DrawLine(int x0, int y0, int x1, int y1, bool is_white);
    void DrawRect(int x0, int y0, int x1, int y1, bool is_white);
    void PrintText(int line_index, const char* text);
    void Render();
};

#include "utils.h"

void shift_buffer(float* buffer, size_t* size) {
  if (*size == 0) {
    return;
  }
  for (size_t i = 1; i < *size; i++) {
    buffer[i - 1] = buffer[i];
  }
  (*size)--;
}

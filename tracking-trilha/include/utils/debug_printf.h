#ifndef DEBUG_PRINTF_H
#define DEBUG_PRINTF_H

#include <stdio.h>

// Macro para printf com flush automático para debug
#define PRINTF_DEBUG(fmt, ...) do { \
    printf(fmt, ##__VA_ARGS__); \
    fflush(stdout); \
} while(0)

// Macro para printf simples (sem flush) - use apenas quando necessário
#define PRINTF_SIMPLE(fmt, ...) printf(fmt, ##__VA_ARGS__)

#endif // DEBUG_PRINTF_H

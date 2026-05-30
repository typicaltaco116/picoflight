#pragma once

#include "pico/stdlib.h"

#include <stdbool.h>
#include <stdint.h>

bool dshot_Init(uint dshot_gpio, bool enable_repeat);
void dshot_SendCommand(uint16_t c);
void dshot_SendThrottle(float t);
uint16_t dshot_GetThrottleCommand(float t);
void dshot_Stop();

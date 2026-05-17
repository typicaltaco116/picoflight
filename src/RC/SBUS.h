#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hardware/uart.h"

#include "RC.h"

void SBUS_InitBus(uart_inst_t *uart, uint32_t rx);

// Currently outputs channels normalized to [0, 1]
bool SBUS_GetChannels(RC_t rc);

bool SBUS_IsFailsafe(void);

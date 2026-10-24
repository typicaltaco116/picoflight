#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <hardware/uart.h>

#include <picoflight/RC/RC.h>

void SBUSInitBus(uart_inst_t *uart, uint32_t rx);

// Currently outputs channels normalized to [0, 1]
bool SBUSGetChannels(RC_t rc);

bool SBUSIsFailsafe(void);

#pragma once

// Local Headers
#include <picoflight/RC/rc.h>

// Pico-SDK Headers
#include <hardware/uart.h>

// Standard Headers
#include <stdint.h>
#include <stdbool.h>

void sbusInitBus(uart_inst_t *uart, uint32_t rx);

// Currently outputs channels normalized to [0, 1]
bool sbusGetChannels(rc_t rc);

bool sbusGetIsFailsafe(void);

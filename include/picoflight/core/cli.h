#pragma once

// Standard Headers
#include <stdint.h>

int32_t cliExecuteString(const char *str);

// Blocks until shell exit.
void cliProcessInput(void);

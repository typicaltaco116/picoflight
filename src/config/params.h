#pragma once

#include "pico/stdlib.h"

#define CONFIGURATION_RESERVED_BYTES (4 * 1024)

void config_RegisterParam(const char *key, uint size);

int config_SetParam(const char *key, void *data);

void* config_GetParam(char *key);

int config_WriteAll(void);

void config_ReadAll(void);

void config_FreeAll(void);

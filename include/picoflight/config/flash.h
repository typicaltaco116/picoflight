#pragma once

#include <pico/stdlib.h>

void configRegisterParam(const char *key, uint size);

int configSetParam(const char *key, void *data);

void* configGetParam(char *key);

int configWriteAll(void);

void configReadAll(void);

void configFreeAll(void);

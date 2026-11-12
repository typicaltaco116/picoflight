#pragma once

// Standard Headers
#include <stdbool.h>
#include <stdint.h>

double map(double x, double inLow, double inHigh, double outLow, double outHigh);
float mapf(float x, float inLow, float inHigh, float outLow, float outHigh);
int mapi(int x, int inLow, int inHigh, int outLow, int outHigh);

double constrain(double x, double min, double max);
float constrainf(float x, float min, float max);
int constraini(int x, int min, int max);

bool exactStrcmp(const char *str1, const char *str2);

uint32_t hashFNV1A32(const char *data, uint32_t size);

// HOLDS STATE BEWARE!!! ONLY USE ONCE.
uint64_t smartDelay(uint64_t delay_us);

float safeCastFloat(uint32_t x);
uint32_t safeCastInt(float x);

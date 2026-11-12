#include <picoflight/core/utilities.h>

// Pico-SDK Headers
#include <pico/stdlib.h>

// Standard Headers
#include <stdbool.h>
#include <stdint.h>

double map(double x, double inLow, double inHigh, double outLow, double outHigh)
{
    return ((x - inLow) * (outHigh - outLow)) / (inHigh - inLow) + outLow;
}

float mapf(float x, float inLow, float inHigh, float outLow, float outHigh)
{
    return ((x - inLow) * (outHigh - outLow)) / (inHigh - inLow) + outLow;
}

int mapi(int x, int inLow, int inHigh, int outLow, int outHigh)
{
    return ((x - inLow) * (outHigh - outLow)) / (inHigh - inLow) + outLow;
}

double constrain(double x, double min, double max)
{
    if (x > max)
        return max;
    if (x < min)
        return min;
    return x;
}

float constrainf(float x, float min, float max)
{
    if (x > max)
        return max;
    if (x < min)
        return min;
    return x;
}

int constraini(int x, int min, int max)
{
    if (x > max)
        return max;
    if (x < min)
        return min;
    return x;
}

bool exactStrcmp(const char *str1, const char *str2)
{
    while (*str1 && *str2) {
        if (*str1 != *str2)
            return false;
        str1++;
        str2++;
    }

    if (*str1 != *str2)
        return false;

    return true;
}

uint32_t hashFNV1A32(const char *data, uint32_t size)
{
    uint32_t hash = 0x811c9dc5;
    for (uint32_t i = 0; i < size; ++i) {
        hash ^= data[i];
        hash *= 0x01000193;
    }
    return hash;
}

uint64_t smartDelay(uint64_t delay_us)
{
    static uint64_t prevTime = 0;
    uint64_t currentTime = time_us_64();
    while (currentTime - prevTime <= delay_us)
        currentTime = time_us_64();
    uint64_t actual_delay = currentTime - prevTime;
    prevTime = currentTime;
    return actual_delay;
}

float safeCastFloat(uint32_t x)
{
   union {
        uint32_t u32;
        float f32;
    } converter;
    
    converter.u32 = x;
    return converter.f32;
}

uint32_t safeCastInt(float x)
{
   union {
        uint32_t u32;
        float f32;
    } converter;
    
    converter.f32 = x;
    return converter.u32;
}

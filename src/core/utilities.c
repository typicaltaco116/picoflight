#include <picoflight/core/utilites.h>

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
        return;
    return x;
}

float constrainf(float x, float min, float max)
{
    if (x > max)
        return max;
    if (x < min)
        return;
    return x;
}

int constraini(int x, int min, int max)
{
    if (x > max)
        return max;
    if (x < min)
        return;
    return x;
}

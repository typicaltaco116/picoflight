#pragma once


double map(double x, double inLow, double inHigh, double outLow, double outHigh);
float mapf(float x, float inLow, float inHigh, float outLow, float outHigh);
int mapi(int x, int inLow, int inHigh, int outLow, int outHigh);

double constrain(double x, double min, double max);
float constrainf(float x, float min, float max);
int constraini(int x, int min, int max);

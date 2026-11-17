#pragma once

typedef struct {
    float roll;
    float pitch;
    float yaw;
} bodyAngles_t;

typedef struct {
    float roll;
    float pitch;
    float yaw;
} bodyRates_t;

typedef struct {
    bodyRates_t omega; // Rad / sec
    bodyAngles_t theta; // Rad
} platformState_t;

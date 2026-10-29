#pragma once

typedef struct {
    float roll;
    float pitch;
    float yaw;
} euler_t;

typedef struct {
    float roll;
    float pitch;
    float yaw;
} rates_t;

typedef struct {
    rates_t omega; // Rad / sec
    euler_t theta; // Rad
} platform_state_t;

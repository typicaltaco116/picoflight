#pragma once

// Standard Headers
#include <stdint.h>

typedef int32_t pwm_t;

pwm_t pwmRegister(int32_t pin);

void pwmInit(pwm_t PWM_obj);

void pwmDrive(pwm_t PWM_obj, float x);

void pwmDriveNorm(pwm_t PWM_obj, float x_norm);

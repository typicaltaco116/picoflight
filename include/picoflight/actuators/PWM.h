#include <stdint.h>

typedef int32_t PWM_t;

PWM_t PWMRegister(int32_t pin);

void PWMInit(PWM_t PWM_obj);

void PWMDrive(PWM_t PWM_obj, float x);

void PWMDriveNorm(PWM_t PWM_obj, float x_norm);

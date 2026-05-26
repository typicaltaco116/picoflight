#include <stdint.h>

typedef int32_t PWM_t;

PWM_t PWM_Register(int32_t pin);

void PWM_Init(PWM_t PWM_obj);

void PWM_Drive(PWM_t PWM_obj, float x);

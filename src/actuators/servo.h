#include <stdint.h>

typedef uint32_t servo_t;

servo_t servo_Register(uint32_t pin);

void servo_Init(servo_t servo);

void servo_Drive(servo_t servo, float x);

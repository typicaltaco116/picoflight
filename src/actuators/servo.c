#include "servo.h"

#include "hardware/pwm.h"
#include "hardware/gpio.h"

#ifdef PICO_RP2350
#define SYSTEM_CLK 150000000.0f
#elif PICO_RP2040
#define SYSTEM_CLK 125000000.0f
#endif

#define PWM_FREQ 50.0f
#define PWM_DIVIDER 64.0f
#define PWM_WRAP ((SYSTEM_CLK / PWM_DIVIDER) / PWM_FREQ)

//#if PWM_WRAP > 65535.0f // 16 bit limit
//#error "PWM wrap for servo not sized correctly."
//#endif

#define UPPER_BOUND_MICROSECONDS 2000.0f
#define LOWER_BOUND_MICROSECONDS 1000.0f

#define UPPER_BOUND_LEVEL ((SYSTEM_CLK / PWM_DIVIDER) * UPPER_BOUND_MICROSECONDS)
#define LOWER_BOUND_LEVEL ((SYSTEM_CLK / PWM_DIVIDER) * LOWER_BOUND_MICROSECONDS)

servo_t servo_Register(uint32_t pin)
{
    return pin;
}

void servo_Init(servo_t servo)
{
    pwm_config config;

    config = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&config, (uint32_t)PWM_DIVIDER);
    pwm_config_set_wrap(&config, (uint16_t)PWM_WRAP);

    pwm_init(PWM_GPIO_SLICE_NUM(servo), &config, true);
    gpio_set_function(servo, GPIO_FUNC_PWM);
}

static float map_float(float x, float L1, float H1, float L2, float H2)
{
    return (x - L1) * (H2 - L2) / (H1 - L1) + H1;
}

void servo_Drive(servo_t servo, float x)
{
    uint16_t level;

    level = (uint16_t)map_float(x, -1, 1, LOWER_BOUND_LEVEL, UPPER_BOUND_LEVEL);

    pwm_set_gpio_level(servo, level);
}

#include "PWM.h"

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

#define UPPER_BOUND_MICROSECONDS 2000.0f
#define LOWER_BOUND_MICROSECONDS 1000.0f

#define UPPER_BOUND_LEVEL ((SYSTEM_CLK / PWM_DIVIDER) * UPPER_BOUND_MICROSECONDS * 1E-6)
#define LOWER_BOUND_LEVEL ((SYSTEM_CLK / PWM_DIVIDER) * LOWER_BOUND_MICROSECONDS * 1E-6)

PWM_t PWM_Register(int32_t pin)
{
    return pin;
}

void PWM_Init(PWM_t PWM_obj)
{
    pwm_config config;

    if (PWM_obj < 0)
        return;

    config = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&config, (uint32_t)PWM_DIVIDER);
    pwm_config_set_wrap(&config, (uint16_t)PWM_WRAP);

    pwm_init(PWM_GPIO_SLICE_NUM(PWM_obj), &config, true);
    gpio_set_function(PWM_obj, GPIO_FUNC_PWM);
}

static float map_float(float x, float L1, float H1, float L2, float H2)
{
    return (x - L1) * (H2 - L2) / (H1 - L1) + L2;
}

void PWM_Drive(PWM_t PWM_obj, float x)
{
    uint16_t level;

    if (PWM_obj < 0)
        return;

    level = (uint16_t)map_float(x, -1.0f, 1.0f, LOWER_BOUND_LEVEL, UPPER_BOUND_LEVEL);

    pwm_set_gpio_level(PWM_obj, level);
}

void PWM_DriveNorm(PWM_t PWM_obj, float x_norm)
{
    uint16_t level;

    if (PWM_obj < 0)
        return;

    level = (uint16_t)map_float(x_norm, 0.0f, 1.0f, LOWER_BOUND_LEVEL, UPPER_BOUND_LEVEL);

    pwm_set_gpio_level(PWM_obj, level);
}

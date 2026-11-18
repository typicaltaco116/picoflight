// Local Headers
#include <picoflight/staticParams.h>
#include <picoflight/core/utilities.h>
#include <picoflight/rc/sbus.h>
#include <picoflight/rc/rcMixer.h>

// Pico-SDK Headers
#include <pico/stdlib.h>

// Standard Headers
#include <stdio.h>

int main(void)
{
    stdio_init_all();

    sbusInitBus(SBUS_UART_INST, SBUS_RX_PIN);

    rcMixerInit();
    rcMixerChannelParameters_t mixParams = (rcMixerChannelParameters_t){
        .enabled = true,
        .trimNorm = 0.0f,
        .inMin = SBUS_MIN_VALUE,
        .inMax = SBUS_MAX_VALUE
    };
    rcMixerSetChannelParameters(1, mixParams);
    rcMixerSetChannelParameters(2, mixParams);
    rcMixerSetChannelParameters(3, mixParams);
    rcMixerSetChannelParameters(4, mixParams);
    rcMixerSetChannelParameters(5, mixParams);
    rcMixerSetChannelParameters(6, mixParams);

    rcMixerSetRates(RATES_ACRO_ROLL_DPS, RATES_ACRO_PITCH_DPS, RATES_ACRO_YAW_DPS);
    rcMixerSetAngleLimits(RATES_ANGLE_ROLL_DEG, RATES_ANGLE_PITCH_DEG);

    while (!stdio_usb_connected());

    while (1) {
        rc_t radioChannels;
        sbusGetChannels(radioChannels);

        bool isFailsafe = sbusGetIsFailsafe();
        bool isArmed = (radioChannels[ARM_CHANNEL] > 0.0f);
        bool isAngle = (radioChannels[MODE_CHANNEL] < 0.0f);
        float throttle = radioChannels[THROT_CHANNEL];

        if (isFailsafe) {
            continue;
        }

        if (isAngle) {
            bodyAngles_t angleSetpoints = rcMixerGetAngleSetpoints(radioChannels);
        }
        else {
            bodyRates_t rateSetpoints = rcMixerGetRateSetpoints(radioChannels);
        }
    }

    return 0;
}

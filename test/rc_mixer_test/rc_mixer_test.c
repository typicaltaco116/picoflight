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

    sleep_ms(10);

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
    rcMixerSetAxisMapping(ROLL_CHANNEL, PITCH_CHANNEL, YAW_CHANNEL);
    rcMixerSetAngleLimits(RATES_ANGLE_ROLL_DEG, RATES_ANGLE_PITCH_DEG);

    while (!stdio_usb_connected());

    bool isFailsafe = true;
    bool isArmed = false;
    bool isAngle;
    bodyAngles_t angleSetpoints;
    bodyRates_t rateSetpoints;

    while (1) {
        smartDelay((1/120.0f) * 1E6);

        rc_t radioChannels;
        if (sbusGetChannels(radioChannels)) {
            isFailsafe = sbusGetIsFailsafe();
            isArmed = (radioChannels[ARM_CHANNEL] > (0.0f - 0.25f));
            isAngle = (radioChannels[MODE_CHANNEL] < 0.0f - 0.25f);
            if (isAngle) {
                angleSetpoints = rcMixerGetAngleSetpoints(radioChannels);
            }
            else {
                rateSetpoints = rcMixerGetRateSetpoints(radioChannels);
            }
        }

        if (isFailsafe) {
            printf("Failsafe  :\n");
            continue;
        }

        if (!isArmed) {
            printf("Disarmed  :\n");
            continue;
        }

        if (isAngle) {
            printf("Angle Mode: %8.2f %8.2f %8.2f\n",
                    angleSetpoints.roll, angleSetpoints.pitch, angleSetpoints.yaw);
        }
        else {
            printf("Acro Mode : %8.2f %8.2f %8.2f\n",
                    rateSetpoints.roll, rateSetpoints.pitch, rateSetpoints.yaw);
        }
    }

    return 0;
}

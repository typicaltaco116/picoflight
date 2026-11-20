// Local Headers
#include <picoflight/sensors/mpu6050.h>
#include <picoflight/sensors/imu.h>
#include <picoflight/estimation/madgwick.h>
#include <picoflight/actuators/dshotx4.h>
#include <picoflight/rc/sbus.h>
#include <picoflight/rc/rcMixer.h>
#include <picoflight/stability/pid.h>
#include <picoflight/actuators/outputMixer.h>
#include <picoflight/core/utilities.h>
#include <picoflight/core/config.h>
#include <picoflight/core/cli.h>
#include <picoflight/staticParams.h>

// pico-sdk Headers
#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <hardware/watchdog.h>

// Standard Headers
#include <stdio.h>


// Global Variables
pid3Axis_t *controller;
bool calibrationPrearm = false;

void picoflightInit(void)
{
    stdio_init_all();

    mpu6050InitBus(
        MPU6050_I2C_INST,
        MPU6050_I2C_SDA_PIN, MPU6050_I2C_SCL_PIN,
        MPU6050_I2C_BAUD
    );
    mpu6050InitDevice(MPU6050_SAMPLE_RATE, MPU6050_G_SCALE, MPU6050_A_SCALE);
    imuSetProcessingScale(MPU6050_G_SCALE, MPU6050_A_SCALE);
    calibrationPrearm = imuGetIsCalibrated();
    if (calibrationPrearm)
        imuLoadCalibration();
    else
        imuSaveCalibrationBlocking(10000);

    madgwickSetStepSize(MADGWICK_STEP_SIZE);

    dshotx4Init(
        DSHOT_PINBASE,
        DSHOT_COUNT,
        DSHOT_SPEED,
        DSHOT_PIO,
        DSHOT_SM
    );
    dshotx4SendThrottles((uint16_t[]){0, 0, 0, 0});

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

    controller = pid3AxisConstructFromFlash(
            (pidGains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Roll Rate
            (pidGains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Roll Angle
            (pidGains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Pitch Rate
            (pidGains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Pitch Angle
            (pidGains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Yaw Rate
            (pidIntegralLimits_t){.iMax = 10.0f, .iMin = -10.0f},
            (pidIntegralLimits_t){.iMax = 10.0f, .iMin = -10.0f},
            (pidIntegralLimits_t){.iMax = 10.0f, .iMin = -10.0f},
            (pidIntegralLimits_t){.iMax = 10.0f, .iMin = -10.0f},
            (pidIntegralLimits_t){.iMax = 10.0f, .iMin = -10.0f}
    );

    outputMixerQuadPinout_t pinout = {
        .m1 = 5, .m2 = 6, .m3 = 7, .m4 = 8,
        .numAux = 0, .aux = NULL
    };
    outputMixerInit(MIXER_QUADCOPTER, &pinout);
}

void handleUSBCLI(void)
{
    if (!stdio_usb_connected())
        return;

    cliProcessInputBlocking(); // Interactive and blocking function.
    watchdog_enable(1 ,1);
    while (1); // Hold until hardware reset.
}

int main(void)
{
    picoflightInit();

    // Main loop.
    uint64_t deltaTime_us = LOOP_PERIOD_US;
    while (1) {
        float dt = (float)deltaTime_us * 1E-6;

        handleUSBCLI();

        imuVectors_t IMU_data = imuGetData();
        platformState_t state = madgwickCompute(IMU_data, dt); 

        rc_t radioChannels;
        sbusGetChannels(radioChannels);

        bool isFailsafe = sbusGetIsFailsafe();
        bool isArmed = !isFailsafe && calibrationPrearm;
        isArmed = isArmed && (radioChannels[ARM_CHANNEL] > 0.0f);
        outputMixerSetArm(isArmed);
        bool isAngle = (radioChannels[MODE_CHANNEL] < 0.0f);

        float throttle = radioChannels[THROT_CHANNEL];

        bodyAngles_t command;
        if (isAngle) {
            bodyAngles_t angleSetpoints = rcMixerGetAngleSetpoints(radioChannels);
            pid3AxisGetAngleResponse(
                    controller,
                    state,
                    dt,
                    angleSetpoints,
                    isArmed,
                    &command.roll,
                    &command.pitch,
                    &command.yaw
            );
        }
        else {
            bodyRates_t rateSetpoints = rcMixerGetRateSetpoints(radioChannels);
            pid3AxisGetRateResponse(
                    controller,
                    state,
                    dt,
                    rateSetpoints,
                    isArmed,
                    &command.roll,
                    &command.pitch,
                    &command.yaw
            );
        }

        float outputs[4];
        outputMixerGetOutputs(command, throttle, outputs);

        dshotx4SendNormThrottles(outputs);

        deltaTime_us = smartDelay(LOOP_PERIOD_US);
    } // end while (true)
}

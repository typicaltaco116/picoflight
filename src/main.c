// Local Headers
#include <picoflight/sensors/mpu6050.h>
#include <picoflight/sensors/imu.h>
#include <picoflight/estimation/madgwick.h>
#include <picoflight/actuators/dshotx4.h>
#include <picoflight/RC/sbus.h>
#include <picoflight/RC/rcMixer.h>
#include <picoflight/stability/pid.h>
#include <picoflight/actuators/mixer.h>
#include <picoflight/core/utilities.h>
#include <picoflight/core/config.h>
#include <picoflight/core/cli.h>

// pico-sdk Headers
#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <hardware/watchdog.h>

// Standard Headers
#include <stdio.h>

// Global Parameters
#define LOOP_FREQ_HZ    (4E3) // 4kHz
#define LOOP_PERIOD_S   (1 / LOOP_FREQ_HZ)
#define LOOP_PERIOD_US  (LOOP_PERIOD_S * 1E6)

// IMU Parameters
#define MPU6050_I2C_SCL_PIN (15)
#define MPU6050_I2C_SDA_PIN (14)
#define MPU6050_I2C_INST    I2C_INSTANCE(1)
#define MPU6050_I2C_BAUD    1000000
#define MPU6050_SAMPLE_RATE MPU6050_1KHZ_RATE
#define MPU6050_G_SCALE     MPU6050_GYRO_SCALE_500
#define MPU6050_A_SCALE     MPU6050_ACCEL_SCALE_2

// Madgwick Parameters
#define MADGWICK_STEP_SIZE 0.2f

// DSHOT Parameters
#define DSHOT_PINBASE   2
#define DSHOT_COUNT     4
#define DSHOT_SPEED     300
#define DSHOT_PIO       pio0
#define DSHOT_SM        0

// SBUS Parameters
#define SBUS_RX_PIN     9
#define SBUS_UART_INST  uart1
#define SBUS_MIN_VALUE  0.2
#define SBUS_MAX_VALUE  0.7

// RC Mapping Parameters
#define ARM_CHANNEL     (5)
#define MODE_CHANNEL    (6)
#define ROLL_CHANNEL    (1)
#define PITCH_CHANNEL   (2)
#define THROT_CHANNEL   (3)
#define YAW_CHANNEL     (4)

// Control Parameters
#define RATES_ACRO_ROLL_DPS     (100.0f)
#define RATES_ACRO_PITCH_DPS    (100.0f)
#define RATES_ACRO_YAW_DPS      (100.0f)
#define RATES_ANGLE_ROLL_DEG    (90.0f)
#define RATES_ANGLE_PITCH_DEG    (90.0f)


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
        imuSaveCalibrationBlocking(10000);
    else
        imuLoadCalibration();

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

    mixerQuadPinout_t pinout = {
        .m1 = 5, .m2 = 6, .m3 = 7, .m4 = 8,
        .numAux = 0, .aux = NULL
    };
    mixerInit(MIXER_QUADCOPTER, &pinout);
}

void handleUSBCLI(void)
{
    if (!stdio_usb_connected())
        return;

    cliProcessInput(); // Interactive and blocking function.
    watchdog_enable(1 ,1);
    while (1); // Hold until hardware reset.
}

int main(void)
{
    picoflightInit();

    // Main loop.
    uint64_t deltaTime_us = LOOP_PERIOD_US;
    while (1) {

        handleUSBCLI();

        imuVectors_t IMU_data = imuGetData();
        platformState_t state = madgwickCompute(IMU_data, LOOP_FREQ_HZ); 

        rc_t radioChannels;
        sbusGetChannels(radioChannels);

        bool isFailsafe = sbusGetIsFailsafe();
        bool isArmed = !isFailsafe && calibrationPrearm;
        isArmed = isArmed && (radioChannels[ARM_CHANNEL] > 0.0f);
        mixerSetArm(isArmed);
        bool isAngle = (radioChannels[MODE_CHANNEL] < 0.0f);

        float throttle = radioChannels[THROT_CHANNEL];

        bodyAngles_t command;
        if (isAngle) {
            bodyAngles_t angleSetpoints = rcMixerGetAngleSetpoints(radioChannels);
            pid3AxisGetAngleResponse(
                    controller,
                    state,
                    (float)deltaTime_us * 1E-6,
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
                    (float)deltaTime_us * 1E-6,
                    rateSetpoints,
                    isArmed,
                    &command.roll,
                    &command.pitch,
                    &command.yaw
            );
        }

        float outputs[4];
        mixerGetOutputs(command, throttle, outputs);

        dshotx4SendNormThrottles(outputs);

        deltaTime_us = smartDelay(LOOP_PERIOD_US);
    } // end while (true)
}

// Local Headers
#include <picoflight/sensors/IMU.h>
#include <picoflight/sensors/IMU_processing.h>
#include <picoflight/estimation/madgwick.h>
#include <picoflight/actuators/dshotx4.h>
#include <picoflight/RC/SBUS.h>
#include <picoflight/RC/RC_mixer.h>
#include <picoflight/stability/PID.h>
#include <picoflight/actuators/mixer.h>

// pico-sdk Headers
#include <pico/stdlib.h>
#include <hardware/i2c.h>

// Global Parameters
#define LOOP_FREQ_HZ    (4E3) // 4kHz
#define LOOP_PERIOD_S   (1 / LOOP_FREQ_HZ)
#define LOOP_PERIOD_US  (LOOP_PERIOD_S * 1E6)

// IMU Parameters
#define MPU6050_I2C_SCL_PIN (15)
#define MPU6050_I2C_SDA_PIN (14)
#define MPU6050_I2C_INST    I2C_INSTANCE(1)
#define MPU6050_I2C_BAUD    1000000
#define MPU6050_SAMPLE_RATE IMU_1KHZ_RATE
#define MPU6050_G_SCALE     IMU_GYRO_SCALE_500
#define MPU6050_A_SCALE     IMU_ACCEL_SCALE_2

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

uint64_t smartDelay(uint64_t delay_us)
{
    static uint64_t prevTime = 0;
    uint64_t currentTime = time_us_64();
    while (currentTime - prevTime <= delay_us)
        currentTime = time_us_64();
    uint64_t actual_delay = currentTime - prevTime;
    prevTime = currentTime;
    return actual_delay;
}

int main(void)
{
    stdio_init_all();

    IMUInitBus(
        MPU6050_I2C_INST,
        MPU6050_I2C_SDA_PIN, MPU6050_I2C_SCL_PIN,
        MPU6050_I2C_BAUD
    );
    IMUInitDevice(MPU6050_SAMPLE_RATE, MPU6050_G_SCALE, MPU6050_A_SCALE);
    IMUSetProcessingScale(MPU6050_G_SCALE, MPU6050_A_SCALE);

    madgwickSetStepSize(MADGWICK_STEP_SIZE);

    dshotx4_t *motors = dshotx4Construct(
        DSHOT_PINBASE,
        DSHOT_COUNT,
        DSHOT_SPEED,
        DSHOT_PIO,
        DSHOT_SM
    );
    dshotx4SendThrottles(motors, (uint16_t[]){0, 0, 0, 0});

    SBUSInitBus(SBUS_UART_INST, SBUS_RX_PIN);

    RCMixerInit();
    RC_mixer_channel_parameters_t mixParams = (RC_mixer_channel_parameters_t){
        .enabled = true,
        .trimNorm = 0.0f,
        .inMin = SBUS_MIN_VALUE,
        .inMax = SBUS_MAX_VALUE
    };
    RCMixerSetChannelParameters(1, mixParams);
    RCMixerSetChannelParameters(2, mixParams);
    RCMixerSetChannelParameters(3, mixParams);
    RCMixerSetChannelParameters(4, mixParams);
    RCMixerSetChannelParameters(5, mixParams);
    RCMixerSetChannelParameters(6, mixParams);

    RCMixerSetRates(RATES_ACRO_ROLL_DPS, RATES_ACRO_PITCH_DPS, RATES_ACRO_YAW_DPS);
    RCMixerSetAngleLimits(RATES_ANGLE_ROLL_DEG, RATES_ANGLE_PITCH_DEG);

    PID_3_axis_t *controller = PID3AxisConstruct(
            (PID_gains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Roll Rate
            (PID_gains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Roll Angle
            (PID_gains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Pitch Rate
            (PID_gains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Pitch Angle
            (PID_gains_t){.p = 1.0f, .i = 0.0f, .d = 1.0f, .all = 1.0f}, // Yaw Rate
            (PID_integral_bounds_t){.iMax = 10.0f, .iMin = -10.0f},
            (PID_integral_bounds_t){.iMax = 10.0f, .iMin = -10.0f},
            (PID_integral_bounds_t){.iMax = 10.0f, .iMin = -10.0f},
            (PID_integral_bounds_t){.iMax = 10.0f, .iMin = -10.0f},
            (PID_integral_bounds_t){.iMax = 10.0f, .iMin = -10.0f}
    );

    quad_pinout_t pinout = {
        .m1 = 5, .m2 = 6, .m3 = 7, .m4 = 8,
        .numAux = 0, .aux = NULL
    };
    mixerInit(MIXER_QUADCOPTER, &pinout);

    // Main loop.
    uint64_t deltaTime_us = LOOP_PERIOD_US;
    while (1) {
        IMU_vectors_t IMU_data = IMUGetData();
        platform_state_t state = madgwickCompute(IMU_data, LOOP_FREQ_HZ); 

        RC_t radioChannels;
        SBUSGetChannels(radioChannels);

        bool isArmed = (radioChannels[ARM_CHANNEL] < 0.0f);
        bool isAngle = (radioChannels[MODE_CHANNEL] < 0.0f);
        float throttle = radioChannels[THROT_CHANNEL];
        mixerSetArm(isArmed);

        euler_t command;
        if (isAngle) {
            euler_t angleSetpoints = RCMixerGetAngleSetpoints(radioChannels);
            PID3AxisGetAngleResponse(
                    controller,
                    state,
                    (float)deltaTime_us * 1E-6,
                    angleSetpoints,
                    &command.roll,
                    &command.pitch,
                    &command.yaw
            );
        }
        else {
            rates_t rateSetpoints = RCMixerGetRateSetpoints(radioChannels);
            PID3AxisGetRateResponse(
                    controller,
                    state,
                    (float)deltaTime_us * 1E-6,
                    rateSetpoints,
                    &command.roll,
                    &command.pitch,
                    &command.yaw
            );
        }

        float outputs[4];
        mixerGetOutputs(command, throttle, outputs);

        dshotx4SendNormThrottles(motors, outputs);

        deltaTime_us = smartDelay(LOOP_PERIOD_US);
    } // end while (true)
}

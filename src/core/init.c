// Primary Header
#include <picoflight/core/init.h>

// Local Headers
#include <picoflight/config/flash.h>
#include <picoflight/sensors/IMU.h>
#include <picoflight/sensors/IMU_processing.h>
#include <picoflight/estimation/madgwick.h>
#include <picoflight/core/core_parameters.h>

// Standard Headers
#include <stdbool.h>


#define FIRST_BOOT_CHECK_VAL 0x67

const uint32_t __attribute__((section(".reserved"))) first_boot_check_val =
    FIRST_BOOT_CHECK_VAL;

static void parameter_registrations(void)
// Check src/config/params.h for a global parameter list
{
    configRegisterParam("first_boot_value", sizeof(uint32_t));

    configRegisterParam("IMU_error", sizeof(IMU_vectors_t));
    configRegisterParam("IMU_is_calibrated", sizeof(bool));

    configRegisterParam("madgwick_step", sizeof(float));
}

static void parameter_set_all_default(void)
{
    configSetParam("first_boot_value", (uint32_t[]){0});

    IMU_vectors_t default_error = {.gyro = {0.0f, 0.0f, 0.0f}, .accel = {0.0f, 0.0f, 0.0f}};
    configSetParam("IMU_error", (IMU_vectors_t[]){default_error});
    configSetParam("IMU_is_calibrated", (bool[]){false});

    configSetParam("madgwick_step", (float[]){0.04f});
}

static void IMU_full_init(void)
{
    IMUInitBus(
        MPU6050_I2C_INST,
        MPU6050_I2C_SDA_PIN, MPU6050_I2C_SCL_PIN,
        MPU6050_I2C_BAUD
    );

    IMUInitDevice(MPU6050_SAMPLE_RATE, MPU6050_G_SCALE, MPU6050_A_SCALE);
    IMUSetProcessingScale(MPU6050_G_SCALE, MPU6050_A_SCALE);

    if (*(bool*)configGetParam("IMU_is_calibrated"))
        IMUSetErrorOffset(*(IMU_vectors_t*)configGetParam("IMU_error"));
    // potentially add arm fail flag here to prevent flying non calibrated
}

static void estimator_full_init(void)
{
    madgwickSetStepSize(*(float*)configGetParam("madgwick_step"));
}

void coreInit(void)
{
    parameter_registrations();
    configReadAll();

    if (*(uint32_t*)configGetParam("first_boot_value") == FIRST_BOOT_CHECK_VAL) {
        parameter_set_all_default();
        configWriteAll();
    }

    IMU_full_init();
    estimator_full_init();
}

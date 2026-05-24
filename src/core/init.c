#include "init.h"

#include <stdbool.h>

#include "src/config/flash.h"
#include "src/sensors/IMU.h"
#include "src/sensors/IMU_processing.h"
#include "src/estimation/madgwick.h"

#include "src/config/pin_mapping.h"
#include "src/config/IMU.h"

#define FIRST_BOOT_CHECK_VAL 0x67

const uint32_t __attribute__((section(".first_boot_value"))) first_boot_check_val =
    FIRST_BOOT_CHECK_VAL;

static void parameter_registrations(void)
// Check src/config/params.h for a global parameter list
{
    config_RegisterParam("first_boot_value", sizeof(uint32_t));

    config_RegisterParam("IMU_error", sizeof(IMU_vectors_t));
    config_RegisterParam("IMU_is_calibrated", sizeof(bool));

    config_RegisterParam("madgwick_step", sizeof(float));
}

static void parameter_set_all_default(void)
{
    config_SetParam("first_boot_value", (uint32_t[]){0});

    IMU_vectors_t default_error = {.gyro = {0.0f, 0.0f, 0.0f}, .accel = {0.0f, 0.0f, 0.0f}};
    config_SetParam("IMU_error", (IMU_vectors_t[]){default_error});
    config_SetParam("IMU_is_calibrated", (bool[]){false});

    config_SetParam("madgwick_step", (float[]){0.04f});
}

static void IMU_full_init(void)
{
    IMU_InitBus(
        MPU6050_I2C_INST,
        MPU6050_I2C_SDA_PIN, MPU6050_I2C_SCL_PIN,
        MPU6050_I2C_BAUD
    );

    IMU_InitDevice(MPU6050_SAMPLE_RATE, MPU6050_G_SCALE, MPU6050_A_SCALE);
    IMU_SetProcessingScale(MPU6050_G_SCALE, MPU6050_A_SCALE);

    if (*(bool*)config_GetParam("IMU_is_calibrated"))
        IMU_SetErrorOffset(*(IMU_vectors_t*)config_GetParam("IMU_error"));
    // potentially add arm fail flag here to prevent flying non calibrated
}

static void estimator_full_init(void)
{
    madgwick_SetStepSize(*(float*)config_GetParam("madgwick_step"));
}

void core_Init(void)
{
    parameter_registrations();
    config_ReadAll();

    if (*(uint32_t*)config_GetParam("first_boot_value") == FIRST_BOOT_CHECK_VAL)
        parameter_set_all_default();
}

#include "IMU_processing.h"

#include <stdint.h>

#include "src/sensors/IMU.h"
#include "src/sensors/MPU6050_register_map.h"

static IMU_vectors_t error = {
    .gyro = {0, 0, 0},
    .accel = {0, 0, 0}
};
static float gyro_scale;
static float accel_scale;


static IMU_vectors_t vector_add(IMU_vectors_t A, IMU_vectors_t B)
{
    IMU_vectors_t sum;

    sum.gyro.x = A.gyro.x + B.gyro.x;
    sum.gyro.y = A.gyro.y + B.gyro.y;
    sum.gyro.z = A.gyro.z + B.gyro.z;
    sum.accel.x = A.accel.x + B.accel.x;
    sum.accel.y = A.accel.y + B.accel.y;
    sum.accel.z = A.accel.z + B.accel.z;

    return sum;
}


IMU_vectors_t IMU_GetData(void)
{
    int16_t gyro_x, gyro_y, gyro_z;
    int16_t accel_x, accel_y, accel_z;
    IMU_vectors_t data;

    IMU_GetRawData(&gyro_x, &gyro_y, &gyro_z, &accel_x, &accel_y, &accel_z);

    data.gyro.x = (float)gyro_x / gyro_scale; // convert to deg / sec
    data.gyro.y = (float)gyro_y / gyro_scale;
    data.gyro.z = (float)gyro_z / gyro_scale;
    data.accel.x = (float)accel_x / accel_scale; // convert to g
    data.accel.y = (float)accel_y / accel_scale;
    data.accel.z = (float)accel_z / accel_scale;

    data = vector_add(data, error);

    return data;
}


void IMU_SetErrorOffset(IMU_vectors_t error_offset)
{
    error = error_offset;
}


IMU_vectors_t IMU_GetCalibrationError(uint32_t cycle_count)
{
    double gyro_x_sum, gyro_y_sum, gyro_z_sum;
    double accel_x_sum, accel_y_sum, accel_z_sum;
    IMU_vectors_t read_data;
    IMU_vectors_t error_offset;

    gyro_x_sum = gyro_y_sum = gyro_z_sum = 0.0;
    accel_x_sum = accel_y_sum = accel_z_sum = 0.0;

    error_offset.gyro.x = 0.0;
    error_offset.gyro.y = 0.0;
    error_offset.gyro.z = 0.0;
    error_offset.accel.x = 0.0;
    error_offset.accel.y = 0.0;
    error_offset.accel.z = 0.0;

    for (uint32_t i = cycle_count; i != 0; --i) {
        read_data = IMU_GetData();

        gyro_x_sum += read_data.gyro.x;
        gyro_y_sum += read_data.gyro.y;
        gyro_z_sum += read_data.gyro.z;
        accel_x_sum += read_data.accel.x;
        accel_y_sum += read_data.accel.y;
        accel_z_sum += read_data.accel.z;
    }

    error_offset.gyro.x = gyro_x_sum / (float)cycle_count;
    error_offset.gyro.y = gyro_y_sum / (float)cycle_count;
    error_offset.gyro.z = gyro_z_sum / (float)cycle_count;
    error_offset.accel.x = accel_x_sum / (float)cycle_count;
    error_offset.accel.y = accel_y_sum / (float)cycle_count;
    error_offset.accel.z = accel_z_sum / (float)cycle_count;

    return error_offset;
}


void IMU_SetProcessingScale(IMU_gyro_scale_e g_scale, IMU_accel_scale_e a_scale)
{
    switch(g_scale) {
        default:
        case IMU_GYRO_SCALE_250:
            gyro_scale = MPU6050_GYRO_250_LSB_SENS;
            break;
        case IMU_GYRO_SCALE_500:
            gyro_scale = MPU6050_GYRO_500_LSB_SENS;
            break;
        case IMU_GYRO_SCALE_1000:
            gyro_scale = MPU6050_GYRO_1000_LSB_SENS;
            break;
        case IMU_GYRO_SCALE_2000:
            gyro_scale = MPU6050_GYRO_2000_LSB_SENS;
            break;
    }

    switch(a_scale) {
        default:
        case IMU_ACCEL_SCALE_2:
            accel_scale = MPU6050_ACCEL_2_LSB_SENS;
            break;
        case IMU_ACCEL_SCALE_4:
            accel_scale = MPU6050_ACCEL_4_LSB_SENS;
            break;
        case IMU_ACCEL_SCALE_8:
            accel_scale = MPU6050_ACCEL_8_LSB_SENS;
            break;
        case IMU_ACCEL_SCALE_16:
            accel_scale = MPU6050_ACCEL_16_LSB_SENS;
            break;
    }
}

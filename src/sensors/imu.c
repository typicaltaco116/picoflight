// Primary Header
#include <picoflight/sensors/imu.h>

// Local Headers
#include <picoflight/sensors/mpu6050.h>
#include <picoflight/sensors/MPU6050_register_map.h>
#include <picoflight/core/config.h>
#include <picoflight/core/utilities.h>

// Standard Headers
#include <stdint.h>

static imuVectors_t _errorVec = {
    .gyro = {0, 0, 0},
    .accel = {0, 0, 0}
};
static float _gyroScale;
static float _accelScale;


static imuVectors_t vector_sub(imuVectors_t A, imuVectors_t B)
{
    imuVectors_t sum;

    sum.gyro.x = A.gyro.x - B.gyro.x;
    sum.gyro.y = A.gyro.y - B.gyro.y;
    sum.gyro.z = A.gyro.z - B.gyro.z;
    sum.accel.x = A.accel.x - B.accel.x;
    sum.accel.y = A.accel.y - B.accel.y;
    sum.accel.z = A.accel.z - B.accel.z;

    return sum;
}


imuVectors_t imuGetData(void)
{
    int16_t gyro_x, gyro_y, gyro_z;
    int16_t accel_x, accel_y, accel_z;
    imuVectors_t data;

    mpu6050GetRawData(&gyro_x, &gyro_y, &gyro_z, &accel_x, &accel_y, &accel_z);

    data.gyro.x = (float)gyro_x / _gyroScale; // convert to deg / sec
    data.gyro.y = (float)gyro_y / _gyroScale;
    data.gyro.z = (float)gyro_z / _gyroScale;
    data.accel.x = (float)accel_x / _accelScale; // convert to g
    data.accel.y = (float)accel_y / _accelScale;
    data.accel.z = (float)accel_z / _accelScale;

    data = vector_sub(data, _errorVec);

    return data;
}


void imuSetErrorOffset(imuVectors_t error_offset)
{
    _errorVec = error_offset;
}


imuVectors_t imuGetCalibrationError(uint32_t cycle_count)
{
    float gyro_x_sum, gyro_y_sum, gyro_z_sum;
    float accel_x_sum, accel_y_sum, accel_z_sum;
    imuVectors_t read_data;
    imuVectors_t error_offset;

    gyro_x_sum = gyro_y_sum = gyro_z_sum = 0.0f;
    accel_x_sum = accel_y_sum = accel_z_sum = 0.0f;

    error_offset.gyro.x = 0.0f;
    error_offset.gyro.y = 0.0f;
    error_offset.gyro.z = 0.0f;
    error_offset.accel.x = 0.0f;
    error_offset.accel.y = 0.0f;
    error_offset.accel.z = 0.0f;

    for (uint32_t i = cycle_count; i != 0; --i) {
        while (!mpu6050IsDataReady());
        read_data = imuGetData();

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
    error_offset.accel.z = accel_z_sum / (float)cycle_count - 1.0f;

    return error_offset;
}


void imuSetProcessingScale(mpu6050GyroScale_e g_scale, mpu6050AccelScale_e a_scale)
{
    switch(g_scale) {
        default:
        case MPU6050_GYRO_SCALE_250:
            _gyroScale = MPU6050_GYRO_250_LSB_SENS;
            break;
        case MPU6050_GYRO_SCALE_500:
            _gyroScale = MPU6050_GYRO_500_LSB_SENS;
            break;
        case MPU6050_GYRO_SCALE_1000:
            _gyroScale = MPU6050_GYRO_1000_LSB_SENS;
            break;
        case MPU6050_GYRO_SCALE_2000:
            _gyroScale = MPU6050_GYRO_2000_LSB_SENS;
            break;
    }

    switch(a_scale) {
        default:
        case MPU6050_ACCEL_SCALE_2:
            _accelScale = MPU6050_ACCEL_2_LSB_SENS;
            break;
        case MPU6050_ACCEL_SCALE_4:
            _accelScale = MPU6050_ACCEL_4_LSB_SENS;
            break;
        case MPU6050_ACCEL_SCALE_8:
            _accelScale = MPU6050_ACCEL_8_LSB_SENS;
            break;
        case MPU6050_ACCEL_SCALE_16:
            _accelScale = MPU6050_ACCEL_16_LSB_SENS;
            break;
    }
}

bool imuGetIsCalibrated(void)
{
    configFetch();
    return configCheckExists("CALIBRATED");
}

void imuLoadCalibration(void)
{
    if (!imuGetIsCalibrated())
        while (1); // Cheap way to kill program.

    configFetch();
    _errorVec.gyro.x = safeCastFloat(configGetEntry("GYRO_ERROR_X"));
    _errorVec.gyro.y = safeCastFloat(configGetEntry("GYRO_ERROR_Y"));
    _errorVec.gyro.z = safeCastFloat(configGetEntry("GYRO_ERROR_Z"));
    _errorVec.accel.x = safeCastFloat(configGetEntry("ACCEL_ERROR_X"));
    _errorVec.accel.y = safeCastFloat(configGetEntry("ACCEL_ERROR_Y"));
    _errorVec.accel.z = safeCastFloat(configGetEntry("ACCEL_ERROR_Z"));
}

#include <stdio.h>
void imuSaveCalibrationBlocking(uint32_t cycles)
{
    configFetch();
    configSafeSetEntry("CALIBRATED", 1);

    _errorVec = (imuVectors_t){{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    _errorVec = imuGetCalibrationError(cycles);
    printf("Saving values: %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f\n",
            _errorVec.gyro.x,
            _errorVec.gyro.y,
            _errorVec.gyro.z,
            _errorVec.accel.x,
            _errorVec.accel.y,
            _errorVec.accel.z
    );
    configSafeSetEntry("GYRO_ERROR_X", safeCastInt(_errorVec.gyro.x));
    configSafeSetEntry("GYRO_ERROR_Y", safeCastInt(_errorVec.gyro.y));
    configSafeSetEntry("GYRO_ERROR_Z", safeCastInt(_errorVec.gyro.z));
    configSafeSetEntry("ACCEL_ERROR_X", safeCastInt(_errorVec.accel.x));
    configSafeSetEntry("ACCEL_ERROR_Y", safeCastInt(_errorVec.accel.y));
    configSafeSetEntry("ACCEL_ERROR_Z", safeCastInt(_errorVec.accel.z));

    configCommit();
}

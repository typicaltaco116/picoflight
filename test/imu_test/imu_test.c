// Local Headers
#include <picoflight/sensors/mpu6050.h>
#include <picoflight/sensors/imu.h>
#include <picoflight/core/cli.h>
#include <picoflight/core/utilities.h>

// PICO-sdk Headers
#include <pico/stdlib.h>
#include <hardware/i2c.h>

// Standard Headers
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// IMU Parameters
#define MPU6050_I2C_SCL_PIN (15)
#define MPU6050_I2C_SDA_PIN (14)
#define MPU6050_I2C_INST    I2C_INSTANCE(1)
#define MPU6050_I2C_BAUD    200E3
#define MPU6050_SAMPLE_RATE MPU6050_8KHZ_RATE
#define MPU6050_G_SCALE     MPU6050_GYRO_SCALE_500
#define MPU6050_A_SCALE     MPU6050_ACCEL_SCALE_2

#define LOOP_RATE           (1E3)
#define LOOP_RATE_INV       (1/LOOP_RATE)

// Global Variables
bool calibrationPrearm = false;

int main() 
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

    while (!stdio_usb_connected());
    cliProcessInputBlocking();

    while (1) {
        imuVectors_t data = imuGetData();

        printf("Gyro = {%6.02f %6.02f %6.02f} Accel = {%6.02f %6.02f %6.02f}\r\n",
                data.gyro.x, data.gyro.y, data.gyro.z,
                data.accel.x, data.accel.y, data.accel.z
        );

        smartDelay(LOOP_RATE_INV * 1E6);
    }
}

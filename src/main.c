// Local Headers
#include <picoflight/sensors/IMU.h>
#include <picoflight/sensors/IMU_processing.h>
#include <picoflight/estimation/madgwick.h>

// pico-sdk Headers
#include <pico/stdlib.h>
#include <hardware/i2c.h>

// Standard Headers

// IMU Parameters
#define MPU6050_I2C_SCL_PIN 15
#define MPU6050_I2C_SDA_PIN 14
#define MPU6050_I2C_INST    I2C_INSTANCE(1)
#define MPU6050_I2C_BAUD    1000000
#define MPU6050_SAMPLE_RATE IMU_1KHZ_RATE
#define MPU6050_G_SCALE     IMU_GYRO_SCALE_500
#define MPU6050_A_SCALE     IMU_ACCEL_SCALE_2

// Madgwick Parameters
#define MADGWICK_STEP_SIZE 0.2f

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
}

// Primary Header
#include <picoflight/sensors/mpu6050.h>

// Local Headers
#include <picoflight/sensors/imu.h>
#include <picoflight/sensors/MPU6050_register_map.h>

// Pico-SDK Headers
#include <pico/stdlib.h>
#include <hardware/i2c.h>

// Standard Headers
#include <stdint.h>
#include <stdlib.h>

static i2c_inst_t *IMU_i2c_instance;
static mpu6050SampleRate_e sample_rate;
static mpu6050GyroScale_e gyro_scale;
static mpu6050AccelScale_e accel_scale;


void mpu6050InitBus(
        i2c_inst_t *i2c,
        uint32_t sda_pin,
        uint32_t scl_pin,
        uint32_t speed)
{
    IMU_i2c_instance = i2c;

    i2c_init(IMU_i2c_instance, speed); 

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

}


void mpu6050InitDevice(
        mpu6050SampleRate_e rate,
        mpu6050GyroScale_e g_scale,
        mpu6050AccelScale_e a_scale)
{
    sample_rate = rate;
    gyro_scale = g_scale;
    accel_scale = a_scale;

    imuSetProcessingScale(g_scale, a_scale);

    // Reset IMU_ device
    mpu6050RegisterWrite(MPU6050_PWR_MGMT_1, MPU6050_RESET_BIT);
    sleep_ms(100);

    // Clear Sleep Bit
    mpu6050RegisterWrite(MPU6050_PWR_MGMT_1, 0x00);
    sleep_ms(10);

    uint8_t gyroConfigValue;
    switch (gyro_scale) {
        default:
        case MPU6050_GYRO_SCALE_250:
            gyroConfigValue = MPU6050_SET_GYRO_SCALE_250;
            break;
        case MPU6050_GYRO_SCALE_500:
            gyroConfigValue = MPU6050_SET_GYRO_SCALE_500;
            break;
        case MPU6050_GYRO_SCALE_1000:
            gyroConfigValue = MPU6050_SET_GYRO_SCALE_1000;
            break;
        case MPU6050_GYRO_SCALE_2000:
            gyroConfigValue = MPU6050_SET_GYRO_SCALE_2000;
            break;
    }
    mpu6050RegisterWrite(MPU6050_GYRO_CONFIG, gyroConfigValue);

    uint8_t accelConfigValue;
    switch (accel_scale) {
        default:
        case MPU6050_ACCEL_SCALE_2:
            accelConfigValue = MPU6050_SET_ACCEL_SCALE_2;
            break;
        case MPU6050_ACCEL_SCALE_4:
            accelConfigValue = MPU6050_SET_ACCEL_SCALE_4;
            break;
        case MPU6050_ACCEL_SCALE_8:
            accelConfigValue = MPU6050_SET_ACCEL_SCALE_8;
            break;
        case MPU6050_ACCEL_SCALE_16:
            accelConfigValue = MPU6050_SET_ACCEL_SCALE_16;
            break;
    }
    mpu6050RegisterWrite(MPU6050_ACCEL_CONFIG, accelConfigValue);

    // sample_rate = Gyro_Rate / divider.
    // Where Gyro rate = 8kHz if DLPF is off and 1kHz if on.
    
    uint8_t dividerValue;
    bool enableDLPF;
    switch (sample_rate) {
        case MPU6050_5HZ_RATE:
            enableDLPF = true;
            dividerValue = 200 - 1; // Implicit 1 attached.
            break;
        default:
        case MPU6050_1KHZ_RATE:
            enableDLPF = true;
            dividerValue = 0;
            break;
        case MPU6050_2KHZ_RATE:
            enableDLPF = false;
            dividerValue = 4 - 1;
            break;
        case MPU6050_4KHZ_RATE:
            enableDLPF = false;
            dividerValue = 2 - 1;
            break;
        case MPU6050_8KHZ_RATE:
            enableDLPF = false;
            dividerValue = 0;
            break;
    }

    if (enableDLPF)
        mpu6050RegisterWrite(MPU6050_CONFIG, MPU6050_DLPF_CFG_6);

    mpu6050RegisterWrite(MPU6050_SMPLRT_DIV, dividerValue);

    // Enable Data Ready Interrupt
    mpu6050RegisterWrite(MPU6050_INT_ENABLE, MPU6050_DATA_RDY_INT_BIT);
}


void mpu6050RegisterWrite(uint8_t address, uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = address;
    buffer[1] = data;
    i2c_write_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, buffer, 2, false);
}


uint8_t mpu6050RegisterRead(uint8_t address)
{
    uint8_t value;
    i2c_write_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, (uint8_t[]){address}, 1, true);
    i2c_read_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, &value, 1, false);
    return value;
}


void mpu6050BufferRead(uint8_t start_address, uint8_t *buffer, size_t length)
{
    i2c_write_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, (uint8_t[]){start_address}, 1, true);
    i2c_read_blocking(IMU_i2c_instance, MPU6050_I2C_ADDR, buffer, length, false);
}


void mpu6050GetRawData(int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z,
                   int16_t* accel_x, int16_t* accel_y, int16_t* accel_z)
{
    uint8_t buffer[6];

    mpu6050BufferRead(MPU6050_GYRO_XOUT_H, (uint8_t*)buffer, 6);

    // Need to swap placement since RP2350 is little endian and MPU6050 outputs
    // as big endian.
    *((uint8_t*)gyro_x + 0) = *(buffer + 1);
    *((uint8_t*)gyro_x + 1) = *(buffer + 0);
    *((uint8_t*)gyro_y + 0) = *(buffer + 3);
    *((uint8_t*)gyro_y + 1) = *(buffer + 2);
    *((uint8_t*)gyro_z + 0) = *(buffer + 5);
    *((uint8_t*)gyro_z + 1) = *(buffer + 4);

    mpu6050BufferRead(MPU6050_ACCEL_XOUT_H, (uint8_t*)buffer, 6);

    *((uint8_t*)accel_x + 0) = *(buffer + 1);
    *((uint8_t*)accel_x + 1) = *(buffer + 0);
    *((uint8_t*)accel_y + 0) = *(buffer + 3);
    *((uint8_t*)accel_y + 1) = *(buffer + 2);
    *((uint8_t*)accel_z + 0) = *(buffer + 5);
    *((uint8_t*)accel_z + 1) = *(buffer + 4);
}


bool mpu6050IsDataReady(void)
{
    return (mpu6050RegisterRead(MPU6050_INT_STATUS) & MPU6050_DATA_RDY_INT_BIT);
}


mpu6050SampleRate_e mpu6050GetCurrentRate(void)
{
    return sample_rate;
}

mpu6050GyroScale_e mpu6050GetCurrentGyroScale(void)
{
    return gyro_scale;
}

mpu6050AccelScale_e IMUGetCurrentAccelScale(void)
{
    return accel_scale;
}

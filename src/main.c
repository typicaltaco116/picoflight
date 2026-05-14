#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "sensors/IMU.h"
#include "IMU_processing.h"
#include "sensor_fusion.h"
#include "orientation_types.h"

static float accel_x_offset;
static float accel_y_offset;
static float accel_z_offset;

int main() 
{
    stdio_init_all();

    IMU_InitBus(i2c_default, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, 200*1000);
    IMU_InitDevice(IMU_5HZ_RATE, IMU_GYRO_SCALE_250, IMU_ACCEL_SCALE_2);

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    while (1) {
        IMU_vectors_t IMU_data;
        euler_t angles;

        while (IMU_IsDataReady());

        IMU_data = IMU_GetData();

        ComputeMadgwick(&angles, IMU_data, 5.0f);

        printf("Euler Angles = {%03.03f %03.03f %03.03f}\r\n",
               angles.roll, angles.pitch, angles.yaw);
    }
}

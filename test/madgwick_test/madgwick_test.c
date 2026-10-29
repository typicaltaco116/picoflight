// Local Headers
#include <picoflight/sensors/IMU.h>
#include <picoflight/sensors/IMU_processing.h>
#include <picoflight/estimation/madgwick.h>
#include <picoflight/orientation/types.h>

// PICO-sdk Headers
#include <pico/stdlib.h>
#include <hardware/i2c.h>

// Standard Headers
#include <stdio.h>
#include <stdint.h>

int main() 
{
    stdio_init_all();

    IMUInitBus(i2c_default, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, 200*1000);
    IMUInitDevice(IMU_5HZ_RATE, IMU_GYRO_SCALE_250, IMU_ACCEL_SCALE_2);

    while (stdio_getchar_timeout_us(100) != (int)'\r');

    while (1) {
        while (IMUIsDataReady());

        IMU_vectors_t IMU_data = IMUGetData();

        platform_state_t state = madgwickCompute(IMU_data, 5.0f);

        printf("State = {%03.03f %03.03f %03.03f %03.03f %03.03f %03.03f}\r\n",
                state.theta.roll, state.theta.pitch, state.theta.yaw,
                state.omega.roll, state.omega.pitch, state.omega.yaw
        );
    }
}

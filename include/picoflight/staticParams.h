#pragma once

// Global Parameters
#define LOOP_FREQ_HZ    (4E3) // 4kHz
#define LOOP_PERIOD_S   (1 / LOOP_FREQ_HZ)
#define LOOP_PERIOD_US  (LOOP_PERIOD_S * 1E6)

// IMU Parameters
#define MPU6050_I2C_SCL_PIN (15)
#define MPU6050_I2C_SDA_PIN (14)
#define MPU6050_I2C_INST    I2C_INSTANCE(1)
#define MPU6050_I2C_BAUD    200E3
#define MPU6050_SAMPLE_RATE MPU6050_4KHZ_RATE
#define MPU6050_G_SCALE     MPU6050_GYRO_SCALE_500
#define MPU6050_A_SCALE     MPU6050_ACCEL_SCALE_2

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
#define SBUS_MIN_VALUE  -0.75
#define SBUS_MAX_VALUE  0.75

// RC Mapping Parameters
#define ARM_CHANNEL     (5)
#define MODE_CHANNEL    (6)
#define ROLL_CHANNEL    (1)
#define PITCH_CHANNEL   (2)
#define THROT_CHANNEL   (3)
#define YAW_CHANNEL     (4)

// Control Parameters
#define RATES_ACRO_ROLL_DPS     (300.0f)
#define RATES_ACRO_PITCH_DPS    (300.0f)
#define RATES_ACRO_YAW_DPS      (250.0f)
#define RATES_ANGLE_ROLL_DEG    (90.0f)
#define RATES_ANGLE_PITCH_DEG    (90.0f)

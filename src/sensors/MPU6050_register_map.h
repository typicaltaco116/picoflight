////////////////////////////////////////////////////////////////////////////////
// MPU6050 i2c IMU Register Map
////////////////////////////////////////////////////////////////////////////////

#define MPU6050_I2C_ADDR            0x68

// Configuration Registers
#define MPU6050_WHO_AM_I_REGISTER   0x75
#define MPU6050_CONFIG              0x1B
#define MPU6050_GYRO_CONFIG         0x1B
#define MPU6050_ACCEL_CONFIG        0x1B
#define MPU6050_INT_ENABLE          0x38
#define MPU6050_INT_STATUS          0x3A
#define MPU6050_SMPLRT_DIV          0x19
#define MPU6050_PWR_MGMT_1          0x6B
#define MPU6050_PWR_MGMT_2          0x6C


// Configuration Macros
#define MPU6050_SET_ACCEL_SCALE_2   (0x0 << 3)  // Assign ACCEL_CONFIG reg  
#define MPU6050_SET_ACCEL_SCALE_4   (0x1 << 3)  // Assign ACCEL_CONFIG reg
#define MPU6050_SET_ACCEL_SCALE_8   (0x2 << 3)  // Assign ACCEL_CONFIG reg
#define MPU6050_SET_ACCEL_SCALE_16  (0x3 << 3)  // Assign ACCEL_CONFIG reg
#define MPU6050_SET_GYRO_SCALE_250  (0x0 << 3)  // Assign GYRO_CONFIG reg  
#define MPU6050_SET_GYRO_SCALE_500  (0x1 << 3)  // Assign GYRO_CONFIG reg
#define MPU6050_SET_GYRO_SCALE_1000 (0x2 << 3)  // Assign GYRO_CONFIG reg
#define MPU6050_SET_GYRO_SCALE_2000 (0x3 << 3)  // Assign GYRO_CONFIG reg

#define MPU6050_DLPF_CFG_0          (0x0)       // Gyro Fs = 8kHz
#define MPU6050_DLPF_CFG_1          (0x1)       // Gyro Fs = 1kHz
#define MPU6050_DLPF_CFG_2          (0x2)       // Gyro Fs = 1kHz
#define MPU6050_DLPF_CFG_3          (0x3)       // Gyro Fs = 1kHz
#define MPU6050_DLPF_CFG_4          (0x4)       // Gyro Fs = 1kHz
#define MPU6050_DLPF_CFG_5          (0x5)       // Gyro Fs = 1kHz
#define MPU6050_DLPF_CFG_6          (0x6)       // Gyro Fs = 1kHz


// Configuration Bitfields
#define MPU6050_DATA_RDY_INT_BIT    (0x1)       // INT_STATUS reg
#define MPU6050_SLEEP_BIT           (0x1 << 6)  // PWR_MGMT_1 reg
#define MPU6050_RESET_BIT           (0x1 << 7)  // PWR_MGMT_1 reg


// Orientiation Data Registers
#define MPU6050_ACCEL_XOUT_H        0x3B
#define MPU6050_ACCEL_XOUT_L        0x3C
#define MPU6050_ACCEL_YOUT_H        0x3D
#define MPU6050_ACCEL_YOUT_L        0x3E
#define MPU6050_ACCEL_ZOUT_H        0x3F
#define MPU6050_ACCEL_ZOUT_L        0x40
#define MPU6050_GYRO_XOUT_H         0x43
#define MPU6050_GYRO_XOUT_L         0x44
#define MPU6050_GYRO_YOUT_H         0x45
#define MPU6050_GYRO_YOUT_L         0x46
#define MPU6050_GYRO_ZOUT_H         0x47
#define MPU6050_GYRO_ZOUT_L         0x48
#define MPU6050_TEMP_OUT_H          0x41
#define MPU6050_TEMP_OUT_L          0x42


// Useful Data Conversion Equations
#define MPU6050_TEMP_CONVERT_DEG_C(reg_val) (reg_val / 340.0f + 36.53f)

#define MPU6050_GYRO_250_LSB_SENS   131.0f  // Units of deg / sec
#define MPU6050_GYRO_500_LSB_SENS   65.5f   // Units of deg / sec
#define MPU6050_GYRO_1000_LSB_SENS  32.8f   // Units of deg / sec
#define MPU6050_GYRO_2000_LSB_SENS  16.4f   // Units of deg / sec
#define MPU6050_ACCEL_2_LSB_SENS    16384.0f// Units of g
#define MPU6050_ACCEL_4_LSB_SENS    8192.0f // Units of g
#define MPU6050_ACCEL_8_LSB_SENS    4096.0f // Units of g
#define MPU6050_ACCEL_16_LSB_SENS   2048.0f // Units of g

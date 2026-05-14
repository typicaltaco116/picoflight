#include <stdint.h>
#include <stdlib.h>
#include "hardware/i2c.h"

void IMU_RegisterWrite(uint8_t address, uint8_t data);

uint8_t IMU_Read(uint8_t address);

void IMU_BufferRead(uint8_t start_address, uint8_t *buffer, size_t length);

void IMU_Init(i2c_inst_t *i2c, uint32_t sda_pin, uint32_t scl_pin, uint32_t speed);

void IMU_GetRawData(int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z,
                   int16_t* accel_x, int16_t* accel_y, int16_t* accel_z);

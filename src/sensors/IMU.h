#include <stdint.h>
#include <stdlib.h>

void IMU_RegisterWrite(uint8_t address, uint8_t data);

uint8_t IMU_Read(uint8_t address);

void IMU_BufferRead(uint8_t start_address, uint8_t *buffer, size_t length);

void IMU_Init(void);

void IMU_GetRawData(int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z,
                   int16_t* accel_x, int16_t* accel_y, int16_t* accel_z);

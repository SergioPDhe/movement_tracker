/*
 * MPU6050.h
 *
 *  Created on: Nov 27, 2022
 *      Author: Sergio
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "I2C.h"

#define MPU6050_ADDR (0x68 << 1)
#define MPU6050_ID 0x68

/////////////// REGISTERS /////////////////////

#define SELF_TEST_X	0x0D
#define SELF_TEST_Y	0x0E
#define SELF_TEST_Z	0x0F
#define SELF_TEST_A	0x10

#define SMPLRT_DIV	0x19
#define CONFIG		0x1A
#define GYRO_CONFIG	0x1B
#define ACCEL_CONFIG	0x1C

#define FIFO_EN		0x23
#define I2C_MST_CTRL	0x24

#define ACCEL_XOUT_H	0x3B
#define ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H	0x3D
#define ACCEL_YOUT_L	0x3E
#define ACCEL_ZOUT_H	0x3F
#define ACCEL_ZOUT_L	0x40

#define ACCEL_XOUT      ACCEL_XOUT_H
#define ACCEL_YOUT      ACCEL_YOUT_H
#define ACCEL_ZOUT      ACCEL_ZOUT_H

#define ACCEL_OUT       ACCEL_XOUT

#define TEMP_OUT_H	0x41
#define TEMP_OUT_L	0x42

#define TEMP_OUT        TEMP_OUT_H

#define GYRO_XOUT_H	0x43
#define GYRO_XOUT_L	0x44
#define GYRO_YOUT_H	0x45
#define GYRO_YOUT_L	0x46
#define GYRO_ZOUT_H	0x47
#define GYRO_ZOUT_L	0x48

#define GYRO_XOUT       GYRO_XOUT_H
#define GYRO_YOUT       GYRO_YOUT_H
#define GYRO_ZOUT       GYRO_ZOUT_H

#define GYRO_OUT        GYRO_XOUT

#define SENSOR_OUT      ACCEL_OUT
#define SENSOR_SIZE     14

#define USER_CTRL	0x6A
#define PWR_MGMT_1	0x6B
#define PWR_MGMT_2	0x6C

#define FIFO_COUNTH	0x72
#define FIFO_COUNTL	0x73
#define FIFO_R_W	0x74
#define WHO_AM_I	0x75

// sensor struct

typedef struct
{
  I2C_HandleTypeDef *i2cHandle;
  uint8_t address;

  uint8_t data[SENSOR_SIZE];    //SENSOR_SIZE = 14

  float accelerometer[3];       // X, Y, Z
  float temp;
  float gyroscope[3];           // X, Y, Z

  float accelOffset[3];
  float gyroOffset[3];
  float tempOffset;
} MPU6050;

uint8_t MPU6050_Init(MPU6050 *dev, I2C_HandleTypeDef *i2cHandle);

HAL_StatusTypeDef MPU6050_ReadTemp(MPU6050 *dev);
HAL_StatusTypeDef MPU6050_ReadAccel(MPU6050 *dev);
HAL_StatusTypeDef MPU6050_ReadGyro(MPU6050 *dev);

uint8_t MPU6050_ReadSensorData(MPU6050 *dev);
uint8_t MPU6050_ReadSensors(MPU6050 *dev);

uint8_t MPU6050_CalculateSensorData(MPU6050 *dev);
uint8_t MPU6050_CalculateAccel(MPU6050 *dev);
uint8_t MPU6050_CalculateTemp(MPU6050 *dev);
uint8_t MPU6050_CalculateGyro(MPU6050 *dev);


HAL_StatusTypeDef MPU6050_ReadRegister(MPU6050 *dev, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef MPU6050_ReadRegisters(MPU6050 *dev, uint8_t reg, uint8_t *data, uint8_t length);
HAL_StatusTypeDef MPU6050_WriteRegister(MPU6050 *dev, uint8_t reg, uint8_t *data);


#endif /* INC_MPU6050_H_ */

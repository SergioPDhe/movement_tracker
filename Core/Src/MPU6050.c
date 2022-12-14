/*
 * MPU6050.c
 *
 *  Created on: Nov 27, 2022
 *      Author: Sergio
 */

#include "main.h"
//#include "usb_device.h"
//#include "usbd_cdc_if.h"
#include "MPU6050.h"


uint8_t MPU6050_Init(MPU6050 *dev, I2C_HandleTypeDef *i2cHandle)
{
  dev->i2cHandle = i2cHandle;
  dev->address = (0x68 << 1);

  for(uint8_t i = 0; i < SENSOR_SIZE; i++)
  {
    dev->data[i] = 0x00;
  }

  dev->accelerometer[0] = 0.0f;
  dev->accelerometer[1] = 0.0f;
  dev->accelerometer[2] = 0.0f;

  dev->gyroscope[0] = 0.0f;
  dev->gyroscope[1] = 0.0f;
  dev->gyroscope[2] = 0.0f;

  dev->temp = 0.0f;


  dev->accelOffset[0] = 0.0f;
  dev->accelOffset[1] = 0.0f;
  dev->accelOffset[2] = 0.0f;

  dev->gyroOffset[0] = 0.0f;
  dev->gyroOffset[1] = 0.0f;
  dev->gyroOffset[2] = 0.0f;

  dev->tempOffset = 0.0f;

  uint8_t errNum = 0;
  HAL_StatusTypeDef status;

  uint8_t regData;

  status = MPU6050_ReadRegister(dev, WHO_AM_I, &regData);
  errNum += (status != HAL_OK);

  if (regData != MPU6050_ID)
  {
    return 255;
  }
  status = MPU6050_WriteRegister(dev, PWR_MGMT_1, 0x00);
  errNum += (status != HAL_OK);
/*
  status = MPU6050_WriteRegister(dev, SMPLRT_DIV, 0x00);
  errNum += (status != HAL_OK);
  status = MPU6050_WriteRegister(dev, CONFIG, 0x00);
  errNum += (status != HAL_OK);

  status = MPU6050_WriteRegister(dev, GYRO_CONFIG, 0x00);
  errNum += (status != HAL_OK);
  status = MPU6050_WriteRegister(dev, ACCEL_CONFIG, 0x00);
  errNum += (status != HAL_OK);*/

  return errNum;

}

HAL_StatusTypeDef MPU6050_ReadTemp(MPU6050 *dev)
{
  uint8_t regData[2];

  HAL_StatusTypeDef status = MPU6050_ReadRegisters(dev, TEMP_OUT_H, regData, 2);

  int16_t tempRaw = ((int16_t)regData[0]<<8) | (int16_t)regData[1];

  dev->temp = ((float)tempRaw/340.0f) + 35.53f;

  return status;
}



HAL_StatusTypeDef MPU6050_ReadAccel(MPU6050 *dev)
{
  uint8_t regData[6];

  HAL_StatusTypeDef status = MPU6050_ReadRegisters(dev, ACCEL_XOUT_H, regData, 6);

  int16_t accelRaw[3];

  accelRaw[0] = ((int16_t)regData[0]<<8) | (int16_t)regData[1];
  accelRaw[1] = ((int16_t)regData[2]<<8) | (int16_t)regData[3];
  accelRaw[2] = ((int16_t)regData[4]<<8) | (int16_t)regData[5];

  dev->accelerometer[0] = ((float)accelRaw[0]/16384.0f);
  dev->accelerometer[1] = ((float)accelRaw[1]/16384.0f);
  dev->accelerometer[2] = ((float)accelRaw[2]/16384.0f);

  return status;
}

HAL_StatusTypeDef MPU6050_ReadGyro(MPU6050 *dev)
{
  uint8_t regData[6];

  HAL_StatusTypeDef status = MPU6050_ReadRegisters(dev, GYRO_XOUT_H, regData, 6);

  int16_t gyroRaw[3];

  gyroRaw[0] = ((int16_t)regData[0]<<8) | (int16_t)regData[1];
  gyroRaw[1] = ((int16_t)regData[2]<<8) | (int16_t)regData[3];
  gyroRaw[2] = ((int16_t)regData[4]<<8) | (int16_t)regData[5];

  dev->gyroscope[0] = ((float)gyroRaw[0]/131.0f);
  dev->gyroscope[1] = ((float)gyroRaw[1]/131.0f);
  dev->gyroscope[2] = ((float)gyroRaw[2]/131.0f);

  return status;
}


uint8_t MPU6050_ReadSensors(MPU6050 *dev)
{
  MPU6050_ReadSensorData(dev);
  MPU6050_CalculateSensorData(dev);

  return 0;
}

uint8_t MPU6050_ReadSensorData(MPU6050 *dev)
{
  MPU6050_ReadRegisters(dev, SENSOR_OUT, dev->data, SENSOR_SIZE);

  return 0;
}

uint8_t MPU6050_CalculateSensorData(MPU6050 *dev)
{
  MPU6050_CalculateAccel(dev);
  MPU6050_CalculateTemp(dev);
  MPU6050_CalculateGyro(dev);

  return 0;
}


uint8_t MPU6050_CalculateAccel(MPU6050 *dev)
{
  int16_t accelRaw[3];

  accelRaw[0] = ((int16_t)dev->data[0]<<8) | (int16_t)dev->data[1];
  accelRaw[1] = ((int16_t)dev->data[2]<<8) | (int16_t)dev->data[3];
  accelRaw[2] = ((int16_t)dev->data[4]<<8) | (int16_t)dev->data[5];

  dev->accelerometer[0] = ((float)accelRaw[0]/16384.0f);
  dev->accelerometer[1] = ((float)accelRaw[1]/16384.0f);
  dev->accelerometer[2] = ((float)accelRaw[2]/16384.0f);

  return 0;
}

uint8_t MPU6050_CalculateTemp(MPU6050 *dev)
{
  int16_t tempRaw = ((int16_t)dev->data[6]<<8) | (int16_t)dev->data[7];

  dev->temp = ((float)tempRaw/340.0f) + 35.53f;

  return 0;
}

uint8_t MPU6050_CalculateGyro(MPU6050 *dev)
{
  int16_t gyroRaw[3];

  gyroRaw[0] = ((int16_t)dev->data[8]<<8) | (int16_t)dev->data[9];
  gyroRaw[1] = ((int16_t)dev->data[10]<<8) | (int16_t)dev->data[11];
  gyroRaw[2] = ((int16_t)dev->data[12]<<8) | (int16_t)dev->data[13];

  dev->gyroscope[0] = ((float)gyroRaw[0]/131.0f);
  dev->gyroscope[1] = ((float)gyroRaw[1]/131.0f);
  dev->gyroscope[2] = ((float)gyroRaw[2]/131.0f);

  return 0;
}


uint8_t MPU6050_ReadRegister(MPU6050 *dev, uint8_t reg, uint8_t *data)
{
  //return HAL_I2C_Mem_Read(dev->i2cHandle, MPU6050_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
  return I2C_ReadByte(dev->i2cHandle, MPU6050_ADDR, reg, data);
}

HAL_StatusTypeDef MPU6050_ReadRegisters(MPU6050 *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
  //return HAL_I2C_Mem_Read(dev->i2cHandle, MPU6050_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);

  return I2C_ReadBytes(dev->i2cHandle, MPU6050_ADDR, reg, data, length);
}
HAL_StatusTypeDef MPU6050_WriteRegister(MPU6050 *dev, uint8_t reg, uint8_t *data)
{
  //return HAL_I2C_Mem_Write(dev->i2cHandle, MPU6050_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
  return I2C_WriteByte(dev->i2cHandle, MPU6050_ADDR, reg, data);
}





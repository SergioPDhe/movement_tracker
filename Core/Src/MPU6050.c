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




HAL_StatusTypeDef MPU6050_ReadRegister(MPU6050 *dev, uint8_t reg, uint8_t *data)
{
  return HAL_I2C_Mem_Read(dev->i2cHandle, MPU6050_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MPU6050_ReadRegisters(MPU6050 *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
  return HAL_I2C_Mem_Read(dev->i2cHandle, MPU6050_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}
HAL_StatusTypeDef MPU6050_WriteRegister(MPU6050 *dev, uint8_t reg, uint8_t *data)
{
  return HAL_I2C_Mem_Write(dev->i2cHandle, MPU6050_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

// START I2C COMMUNICATION
int MPU6050_I2CStart(MPU6050 *dev)
{
  dev->i2cHandle->Instance->CR1 |= I2C_CR1_ACK; // set ACK bit

  dev->i2cHandle->Instance->CR1 |= I2C_CR1_START; // set START bit to generate start condition

  while (!(dev->i2cHandle->Instance->SR1 & I2C_SR1_SB)); // wait for SB to set
}

// RESTART I2C COMMUNICATION
int MPU6050_I2CStart(MPU6050 *dev)
{
  dev->i2cHandle->Instance->CR1 |= I2C_CR1_START; // set START bit to generate start condition

  while (!(dev->i2cHandle->Instance->SR1 & I2C_SR1_SB)); // wait for SB to set
}

// SEND I2C SLAVE ADDRESS
int MPU6050_I2CAddress(MPU6050 *dev, char action)
{
  uint8_t address = dev->address; // "write" by default

  if(action == 'r' || action == 'R')
  {
    address |= 1; // set read/write bit to 'read'
  }

  dev->i2cHandle->Instance->DR = address; // send address

  while (!(dev->i2cHandle->Instance->SR1 & I2C_SR1_ADDR)); // wait for ADDR bit to set
  uint8_t temp = (dev->i2cHandle->Instance->SR1);
  temp = (dev->i2cHandle->Instance->SR2);// read SR1 and SR2 to clear ADDR bit
}

// WRITE I2C DATA
int MPU6050_I2CWriteMany(MPU6050 *dev, uint8_t data, int length)
{
  while (length>0)
  {
    while (!(dev->i2cHandle->Instance->SR1 & I2C_SR1_TXE)); // wait for TXE bit to set (TX empty)

    dev->i2cHandle->Instance->DR = data;	// send data into Data Register

    length--;	// decrement length
  }
  while (!(dev->i2cHandle->Instance->SR1 & I2C_SR1_BTF)); // wait for BTF bit to set (done transmitting)
}

int MPU6050_I2CWrite(MPU6050 *dev, uint8_t data, int length=1)
{

  while (!(dev->i2cHandle->Instance->SR1 & I2C_SR1_TXE)); // wait for TXE bit to set (TX empty)

  dev->i2cHandle->Instance->DR = data;	// send data into Data Register

  while (!(dev->i2cHandle->Instance->SR1 & I2C_SR1_BTF)); // wait for BTF bit to set (done transmitting)

}


int MPU6050_I2CRead(MPU6050 *dev)
{

}


int MPU6050_I2CStop(MPU6050 *dev)
{
  dev->i2cHandle->Instance->CR1 |= I2C_CR1_STOP; // set STOP bit to generate stop condition
}


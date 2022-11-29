/*
 * I2C.c
 *
 *  Created on: Nov 29, 2022
 *      Author: Sergio
 */

#include "I2C.h"


// START I2C COMMUNICATION
uint8_t I2C_Start(I2C_HandleTypeDef *i2cHandle)
{
  i2cHandle->Instance->CR1 |= I2C_CR1_ACK; // set ACK bit

  i2cHandle->Instance->CR1 |= I2C_CR1_START; // set START bit to generate start condition

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_SB)); // wait for SB to set
}

// RESTART I2C COMMUNICATION
uint8_t I2C_Restart(I2C_HandleTypeDef *i2cHandle)
{
  i2cHandle->Instance->CR1 |= I2C_CR1_START; // set START bit to generate start condition

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_SB)); // wait for SB to set
}

// SEND I2C SLAVE ADDRESS
uint8_t I2C_AddressRead(I2C_HandleTypeDef *i2cHandle, uint8_t address)
{
  address |= 0x01; // set read/write bit to 'read'
  dev->i2cHandle->Instance->DR = address; // send address

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_ADDR)); // wait for ADDR bit to set

  uint8_t temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);	// read SR1 and SR2 to clear ADDR bit
}

// SEND ADDRESS FOR WRITING
uint8_t I2C_AddressWrite(I2C_HandleTypeDef *i2cHandle, uint8_t address)
{
  i2cHandle->Instance->DR = address; // send address

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_ADDR)); // wait for ADDR bit to set

  uint8_t temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);	// read SR1 and SR2 to clear ADDR bit
}

// SEND ADDRESS FOR READING
uint8_t I2C_Stop(I2C_HandleTypeDef *i2cHandle)
{
  i2cHandle->Instance->CR1 |= I2C_CR1_STOP; // set STOP bit to generate stop condition
}




uint8_t I2C_SendByte(I2C_HandleTypeDef *i2cHandle, uint8_t *data)
{

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_TXE)); // wait for TXE bit to set (TX empty)

  i2cHandle->Instance->DR = *data;	// send data into Data Register

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_BTF)); // wait for BTF bit to set (done transmitting)

}

uint8_t I2C_SendBytes(I2C_HandleTypeDef *i2cHandle, uint8_t *data, uint16_t size)
{
  uint16_t i;
  for(i = 0; i<size; i++)
  {
    while (!(i2cHandle->Instance->SR1 & I2C_SR1_TXE)); // wait for TXE bit to set (TX empty)

    i2cHandle->Instance->DR = data[i];	// send data into Data Register
  }
  while (!(i2cHandle->Instance->SR1 & I2C_SR1_BTF)); // wait for BTF bit to set (done transmitting)
}


uint8_t I2C_ReadByte(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data)
{
  I2C_Start(i2cHandle);
  I2C_AddressWrite(i2cHandle, deviceAddress);
  I2C_SendByte(i2cHandle, memoryAddress);

  I2C_Restart(i2cHandle);
  I2C_AddressRead(i2cHandle, deviceAddress);

  I2C_SendByte(i2cHandle, data);
  I2C_Stop(i2cHandle);
}


uint8_t I2C_ReadBytes(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data, uint16_t size);

uint8_t I2C_WriteByte(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data)
{
  I2C_Start(i2cHandle);
  I2C_AddressWrite(i2cHandle, deviceAddress);
  I2C_SendByte(i2cHandle, memoryAddress);
  I2C_SendByte(i2cHandle, data);
  I2C_Stop(i2cHandle);
}
uint8_t I2C_WriteBytes(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data, uint16_t size)
{
  I2C_Start(i2cHandle);
  I2C_AddressWrite(i2cHandle, deviceAddress);
  I2C_SendByte(i2cHandle, memoryAddress);
  I2C_SendBytes(i2cHandle, data, size);
  I2C_Stop(i2cHandle);
}

uint8_t I2C_ReadByte(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data);
uint8_t I2C_ReadBytes(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data, uint16_t size);




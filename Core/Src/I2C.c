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

  return 0;
}

// RESTART I2C COMMUNICATION
uint8_t I2C_Restart(I2C_HandleTypeDef *i2cHandle)
{
  i2cHandle->Instance->CR1 |= I2C_CR1_START; // set START bit to generate start condition

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_SB)); // wait for SB to set

  return 0;
}

// SEND I2C SLAVE ADDRESS
uint8_t I2C_AddressRead(I2C_HandleTypeDef *i2cHandle, uint8_t address)
{
  address |= 0x01; // set read/write bit to 'read'
  i2cHandle->Instance->DR = address; // send address

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_ADDR)); // wait for ADDR bit to set

  uint8_t temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);	// read SR1 and SR2 to clear ADDR bit

  return 0;
}

uint8_t I2C_AddressReadNACK(I2C_HandleTypeDef *i2cHandle, uint8_t address)
{
  address |= 0x01; 					// set read/write bit to 'read'
  i2cHandle->Instance->DR = address; 		// send address

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_ADDR)); 	// wait for ADDR bit to set

  i2cHandle->Instance->CR1 &= ~I2C_CR1_ACK;		// disable ACK

  uint8_t temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);	// read SR1 and SR2 to clear ADDR bit

  return 0;
}

// SEND ADDRESS FOR WRITING
uint8_t I2C_AddressWrite(I2C_HandleTypeDef *i2cHandle, uint8_t address)
{
  i2cHandle->Instance->DR = address; // send address

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_ADDR)); // wait for ADDR bit to set

  uint8_t temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);	// read SR1 and SR2 to clear ADDR bit

  return 0;
}


// SEND STOP CONDITION
uint8_t I2C_Stop(I2C_HandleTypeDef *i2cHandle)
{
  i2cHandle->Instance->CR1 |= I2C_CR1_STOP; // set STOP bit to generate stop condition

  return 0;
}


uint8_t I2C_SendByte(I2C_HandleTypeDef *i2cHandle, uint8_t *data)
{

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_TXE)); // wait for TXE bit to set (TX empty)

  i2cHandle->Instance->DR = *data;	// send data into Data Register

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_BTF)); // wait for BTF bit to set (done transmitting)

  return 0;

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

  return 0;
}


uint8_t I2C_ReadByte(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data)
{
  I2C_RequestRead(i2cHandle, deviceAddress, memoryAddress);	// Request specific device and memory address to be read

  I2C_Restart(i2cHandle);					// Send (Re)Start condition

  I2C_AddressReadNACK(i2cHandle, deviceAddress);		// Send device address and disable ACK

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_RXNE)); 		// wait for RXNE bit to set (RX NOT empty)

  *data = (i2cHandle->Instance->DR & 0x00FF);			// read Data register

  I2C_Stop(i2cHandle);						// STOP condition

  return 0;
}


uint8_t I2C_ReadBytes(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data, uint16_t size);

uint8_t I2C_WriteByte(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data)
{
  I2C_Start(i2cHandle);
  I2C_AddressWrite(i2cHandle, deviceAddress);
  I2C_SendByte(i2cHandle, &memoryAddress);
  I2C_SendByte(i2cHandle, data);
  I2C_Stop(i2cHandle);

  return 0;
}
uint8_t I2C_WriteBytes(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data, uint16_t size)
{
  I2C_Start(i2cHandle);
  I2C_AddressWrite(i2cHandle, deviceAddress);
  I2C_SendByte(i2cHandle, &memoryAddress);
  I2C_SendBytes(i2cHandle, data, size);
  I2C_Stop(i2cHandle);

  return 0;
}

// START COMMUNICATION AND REQUEST DATA FROM DEVICE MEMORY
uint8_t I2C_RequestRead(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress)
{
  I2C_Start(i2cHandle);				// Start I2C communication
  I2C_AddressWrite(i2cHandle, deviceAddress);	// Send device address
  I2C_SendByte(i2cHandle, &memoryAddress);	// Send memory address of device to be read

  return 0;
}





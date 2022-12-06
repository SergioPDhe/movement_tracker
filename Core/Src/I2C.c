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
  i2cHandle->Instance->CR1 &= ~I2C_CR1_POS; // clear POS bit

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

  //uint8_t temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);	// read SR1 and SR2 to clear ADDR bit

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

uint8_t I2C_AddressReadNACKPOS(I2C_HandleTypeDef *i2cHandle, uint8_t address)
{
  address |= 0x01; 					// set read/write bit to 'read'
  i2cHandle->Instance->DR = address; 		// send address

  while (!(i2cHandle->Instance->SR1 & I2C_SR1_ADDR)); 	// wait for ADDR bit to set

  i2cHandle->Instance->CR1 &= ~I2C_CR1_ACK;		// disable ACK
  i2cHandle->Instance->CR1 |= I2C_CR1_POS;		// set POS

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


uint8_t I2C_ReadData(I2C_HandleTypeDef *i2cHandle, uint8_t *data)
{
  while (!(i2cHandle->Instance->SR1 & I2C_SR1_RXNE)); 		// wait for RXNE bit to set (RX NOT empty)

  *data = (i2cHandle->Instance->DR & 0x00FF);			// read Data register

  return 0;
}

uint8_t I2C_ReceiveBytes(I2C_HandleTypeDef *i2cHandle, uint8_t *data, uint16_t size)
{
  uint8_t temp; // dummy variable

  if (size == 0)							// size of data is 0 (nothing)
  {
    temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);	// read SR1 and SR2 to clear ADDR bit
    i2cHandle->Instance->CR1 |= I2C_CR1_STOP;
  }
  else if (size == 1)							// size of data is 1 single byte
  {
    i2cHandle->Instance->CR1 &= ~I2C_CR1_ACK;					// disable ACK
    temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);		// read SR1 and SR2 to clear ADDR bit
    i2cHandle->Instance->CR1 |= I2C_CR1_STOP;					// set STOP condition
  }
  else if (size == 2)							// size is 2 bytes
  {
    i2cHandle->Instance->CR1 &= ~I2C_CR1_ACK;					// disable ACK
    i2cHandle->Instance->CR1 |= I2C_CR1_POS;					// set POS
    temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);		// read SR1 and SR2 to clear ADDR bit
  }
  else									// size > 2
  {
    temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);		// read SR1 and SR2 to clear ADDR bit
  }

  while (size > 0)					// while size > 0
  {
    if (size <= 3)						// if size <= 3 (last 1-3 bytes)
    {
      if (size == 1)							// if size == 1
      {
	while (!(i2cHandle->Instance->SR1 & I2C_SR1_RXNE)); 			// wait for RXNE bit to set (RX NOT empty)

	*data = i2cHandle->Instance->DR;					// read Data register to data pointer
	data++;									// increment data pointer
	size--;									// decrement data left to read
      }
      else if (size == 2)
      {
	while (!(i2cHandle->Instance->SR1 & I2C_SR1_BTF)); 			// wait for BTF bit to set (done transmitting)
							      // BTF is set when a byte is received but DR was still not read,
								// meaning there are 2 bytes ready to be read (one in DR and the one in the shift register)
								// BTF also stops the clock, stopping all communication until it is cleared and ready to read more data

	i2cHandle->Instance->CR1 |= I2C_CR1_STOP;				// set STOP condition

	*data = i2cHandle->Instance->DR;					// read Data register to data pointer
	data++;									// increment data pointer
	size--;									// decrement data left to read

	*data = i2cHandle->Instance->DR;					// read Data register to data pointer
	data++;									// increment data pointer
	size--;									// decrement data left to read
      }
      else	// size == 3
      {
	while (!(i2cHandle->Instance->SR1 & I2C_SR1_BTF)); 			// wait for BTF bit to set (done transmitting)
	                                                    // Here byte (N-2) is in the DR and (N-1) is in the shift register
	i2cHandle->Instance->CR1 &= ~I2C_CR1_ACK;				// disable ACK

	*data = i2cHandle->Instance->DR;					// read Data register to data pointer
	data++;									// increment data pointer
	size--;									// decrement data left to read

	while (!(i2cHandle->Instance->SR1 & I2C_SR1_BTF)); 			// wait for BTF bit to set (done transmitting)
	i2cHandle->Instance->CR1 |= I2C_CR1_STOP;				// set STOP condition

	*data = i2cHandle->Instance->DR;					// read Data register to data pointer (N-1)
	data++;									// increment data pointer
	size--;									// decrement data left to read

	*data = i2cHandle->Instance->DR;					// read Data register to data pointer (N)
	data++;									// increment data pointer
	size--;
      }
    }
    else 							// size > 3
    {
      while (!(i2cHandle->Instance->SR1 & I2C_SR1_RXNE));                         // wait for RXNE bit to set (RX NOT empty)

      /*if(i2cHandle->Instance->SR1 & I2C_SR1_BTF)                        // if BTF is set (there are 2 datapoints ready for reading)
      {
        *data = i2cHandle->Instance->DR;                                          // read Data register to data pointer
        data++;                                                                   // increment data pointer
        size--;                                                                   // decrement data left to read
      }*/

      *data = i2cHandle->Instance->DR;						// read Data register to data pointer
      data++;									// increment data pointer
      size--;									// decrement data left to read
    }
  }

  return 0;
}


uint8_t I2C_ReadByte(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data)
{
  I2C_RequestRead(i2cHandle, deviceAddress, memoryAddress);	// Request specific device and memory address to be read

  I2C_Restart(i2cHandle);					// Send (Re)Start condition

  I2C_AddressRead(i2cHandle, deviceAddress);				// Send device address

  i2cHandle->Instance->CR1 &= ~I2C_CR1_ACK;				// disable ACK
  uint8_t temp = (i2cHandle->Instance->SR1 | i2cHandle->Instance->SR2);	// read SR1 and SR2 to clear ADDR bit

  I2C_Stop(i2cHandle);						// STOP condition

  I2C_ReadData(i2cHandle, data);				// Read byte from Data Register

  return 0;
}


uint8_t I2C_ReadBytes(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *data, uint16_t size)
{
  I2C_RequestRead(i2cHandle, deviceAddress, memoryAddress);	// Request specific device and memory address to be read

  I2C_Restart(i2cHandle);					// Send (Re)Start condition

  I2C_AddressRead(i2cHandle, deviceAddress);		        // Send device address and disable ACK

  I2C_ReceiveBytes(i2cHandle, data, size);			// Read series of bytes

  return 0;
}

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





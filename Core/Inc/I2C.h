/*
 * I2C.h
 *
 *  Created on: Nov 29, 2022
 *      Author: Sergio
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32f4xx_hal.h"

uint8_t I2C_Start(I2C_HandleTypeDef *i2cHandle);
uint8_t I2C_Restart(I2C_HandleTypeDef *i2cHandle);
uint8_t I2C_AddressRead(I2C_HandleTypeDef *i2cHandle, uint8_t address);
uint8_t I2C_AddressWrite(I2C_HandleTypeDef *i2cHandle, uint8_t address);
uint8_t I2C_Stop(I2C_HandleTypeDef *i2cHandle);

uint8_t I2C_SendByte(I2C_HandleTypeDef *i2cHandle);
uint8_t I2C_SendBytes(I2C_HandleTypeDef *i2cHandle);
uint8_t I2C_ReceiveByte(I2C_HandleTypeDef *i2cHandle);
uint8_t I2C_ReceiveBytes(I2C_HandleTypeDef *i2cHandle);

#endif /* INC_I2C_H_ */

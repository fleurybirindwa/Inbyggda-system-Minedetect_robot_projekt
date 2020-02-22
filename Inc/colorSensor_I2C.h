/*
 * colorSensor_I2C.h
 *
 *  Created on: 23 jan. 2019
 *      Author: William Jangren
 */

#ifndef COLORSENSOR_I2C_H_
#define COLORSENSOR_I2C_H_

#include "stm32f4xx_hal.h"

void colorSensor_Init(void);
void colorSensor_WriteRegister(uint8_t, uint8_t);
void getRGBValue();
uint8_t colorSensor_ReadRegister(uint8_t);
uint16_t colorSensor_ReadRegisters(uint8_t);

#endif /* COLORSENSOR_I2C_H_ */

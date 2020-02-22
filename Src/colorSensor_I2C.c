
#include "main.h"
#include "stm32f4xx_hal.h"
#include "colorSensor_I2C.h"

#define cs_address 0x29<<1

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart6;
extern int calibrateCount;

int clear,red,green,blue;

char strr[30];

// i2c function for init the color sensor, writing and reading one or more registers.
void colorSensor_Init(void){
	colorSensor_WriteRegister(0x01,0xF6);
	colorSensor_WriteRegister(0x03,0xFF);
	colorSensor_WriteRegister(0x0f,0x00);
	colorSensor_WriteRegister(0x00,0x1B);
}

//i2c function used to write to the colorsensor
void colorSensor_WriteRegister(uint8_t reg, uint8_t value){
	uint8_t data[2];
	data[0] = reg|0x80;
	data[1] = value;
	HAL_I2C_Master_Transmit(&hi2c1, cs_address, data, 2, 100);
}

//i2c function to read from multiple registers
uint16_t colorSensor_ReadRegisters(uint8_t reg){
	uint8_t addr = reg|0x80;
	HAL_I2C_Master_Transmit(&hi2c1, cs_address, &addr, 1, 10);
	uint8_t data[2];
	HAL_I2C_Master_Receive(&hi2c1,cs_address,&data,2,10);
	uint16_t value = (((int)data[1])<<8)|data[0];
	return value;
}

//i2c function to read from a single register
uint8_t colorSensor_ReadRegister(uint8_t reg){
	uint8_t addr = reg|0x80;
	HAL_I2C_Master_Transmit(&hi2c1,0x29<<1, &addr, 1, 10);
	uint8_t data;
	HAL_I2C_Master_Receive(&hi2c1,0x29<<1,&data,1,10);
	return data;
}

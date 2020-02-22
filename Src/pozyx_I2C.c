

#include "main.h"
#include "stm32f4xx_hal.h"
#include "pozyx_I2C.h"
#include "Pozyx.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern int pozTick;

uint8_t data[3];
uint8_t Xposdata[4];
uint8_t Yposdata[4];
uint32_t posX,posY;

state_pozyx pozyxState = STATE_POZ_IDLE;

// This function initializes the Pozyx
void pozyx_Init(){
	HAL_I2C_Mem_Read(&hi2c1,POZYX_I2C_ADDRESS,POZYX_RESET_SYS, 1, data, 1, 100);
	if(pozTick >= 1){
		HAL_I2C_Mem_Read(&hi2c1,POZYX_I2C_ADDRESS,POZYX_DEVICES_DISCOVER, 1, data, 1, 100);
	}
	pozTick=0;
}

/* This statemachine first tells the Pozyx to do a "do_positioning".
   After a delay it then reads the x and y coordinates from pos_x and pos_y into Xposdata and Yposdata */
void Pozyx_ReadRegisters(){
	switch (pozyxState){
		case STATE_POZ_IDLE:
			if(pozTick >= 2){
				HAL_I2C_Mem_Read(&hi2c1,POZYX_I2C_ADDRESS, POZYX_DO_POSITIONING, 1, data, 1, 100);
				pozyxState=STATE_POZ_ONE;
				pozTick=0;
			}
			break;

		case STATE_POZ_ONE:
			if(pozTick >= 4){
				HAL_I2C_Mem_Read(&hi2c1,POZYX_I2C_ADDRESS, POZYX_POS_X, 1, Xposdata, 4, 100);
				HAL_I2C_Mem_Read(&hi2c1,POZYX_I2C_ADDRESS, POZYX_POS_Y, 1, Yposdata, 4, 100);
				posX = Xposdata[3] << 24 | Xposdata[2] << 16 | Xposdata[1] << 8 | Xposdata[0];
				posY = Yposdata[3] << 24 | Yposdata[2] << 16 | Yposdata[1] << 8 | Yposdata[0];
				pozyxState=STATE_POZ_IDLE;
				pozTick=0;
			}
			break;

		default:
			break;
	}
}


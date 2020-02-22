
#include "BLE_sendData.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "pozyx_I2C.h"

extern UART_HandleTypeDef huart6;
extern uint32_t posX, posY;
extern int red,green,blue;

char str[40];

/*--------------------------------------------------------------------------------------------------------------
 * BLE_sendData(typeofSend)
  Function to transmit a message over uart6. typeofSend is a number between 1-8 to print the desired message
 --------------------------------------------------------------------------------------------------------------- */
void BLE_sendData(int typeofSend){
	switch(typeofSend) {
	case 1:
		sprintf(str,"RGB(%d,%d,%d)",red,green,blue); //mina
	   break;

	case 2:
		sprintf(str,"1"); //hinder
	   break;

	case 3:
		sprintf(str,"0"); //calibration
		break;

	case 4:
		sprintf(str,"_(%d,%d)", posX, posY); // Buzzer on
		break;

	case 5:
		sprintf(str,"3"); // Buzzer off
		break;

	case 6:
		sprintf(str,"4"); // Robot on
		break;

	case 7:
		sprintf(str,"5"); // Robot off
		break;

	case 8:
		sprintf(str,"CORD:(%d,%d,0)", posX, posY);
		break;

	default :
	   break;
	}
	HAL_UART_Transmit_IT(&huart6,str,strlen(str));
}


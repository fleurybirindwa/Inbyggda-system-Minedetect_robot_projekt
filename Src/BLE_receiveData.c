

#include "BLE_ReceiveData.h"
#include "BLE_sendData.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "motor_control.h"
#include "buzzer_pwm.h"
#include "mineSearching.h"

BLE_state_e BLE_State = STATE_BLE_IDLE;

extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim2;
extern motorState_t state;

extern int enablemotorQueue;
extern int colorSensor_enable;
extern int ledBlink;

int enableBuzzer=0;
int startRobot=0;

uint8_t BLE_buffer_rx[1];

/*--------------------------------------------------------------------------------------------------------------
 * BLE_Statemachine()
  Statemachine that changes the current state depending of the data in BLE_buffer_rx
 --------------------------------------------------------------------------------------------------------------- */
void BLE_Statemachine(){
	switch (BLE_State){
	case STATE_BLE_IDLE:
		if(BLE_buffer_rx[0] == '6'){      // 6 = Start robot
			BLE_State=STATE_BLE_START;
		}
		else if(BLE_buffer_rx[0] == '8'){ // 8 = Buzz robot
			BLE_State=STATE_BLE_BUZZER;
		}
		else if(BLE_buffer_rx[0] == '2'){ // 2 = Stop robot
			BLE_State=STATE_BLE_STOP;
		}
		BLE_buffer_rx[0]='0';
		break;

	case STATE_BLE_BUZZER:
		if(enableBuzzer==0){
			BLE_sendData(4);
			HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3); // Start buzzer
			enableBuzzer=1;
			ledBlink=1;
		}
		else if(enableBuzzer==1){
			BLE_sendData(5);
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_3); // Stop buzzer
			enableBuzzer=0;
			ledBlink=0;
			setColorRGB(0,0,250,250,250);
		}
		BLE_State=STATE_BLE_IDLE;
		break;

	case STATE_BLE_STOP:
		state = STATE_STOP;
		BLE_State=STATE_BLE_IDLE;
		enablemotorQueue=0;
		startRobot=0;
		BLE_sendData(7);
		break;

	case STATE_BLE_START:
		state = STATE_FORWARD;
		BLE_State=STATE_BLE_IDLE;
		enablemotorQueue=1;
		startRobot=1;
		BLE_sendData(6);
		break;

	default:
		break;
	}
}

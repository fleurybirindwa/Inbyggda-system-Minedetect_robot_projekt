
#include "motor_control.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "event_queue.h"
#include "BLE_sendData.h"
#include "BLE_receiveData.h"

motorState_t state = STATE_STOP;
event_e event;

int enablemotorQueue=0;
int direction=1;
int motorSpeed = 49;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern int delayTick;
extern int colorSensor_enable;
extern int distSensor_Enable;
extern int enableBuzzer;
extern int buttonLeft,buttonRight;

/* This is an event driven statemachine that reads an event from the myQueue.
 * Depending on which event that is read from the queue, the matching state is set. */

void motorStateMachine(myQueue *Q){
	if(enablemotorQueue==1){
		//Read from the Queue and set a state
		event = readQueue(Q);
		if(event == EVENT_START){
			state = STATE_ACCELERATE;
			delayTick=0;
		}
		else if(event == EVENT_STOP){
			state = STATE_STOP;
		}
		else if(event == EVENT_OBSTACLE_LEFT){
			state = STATE_OBSTACLE_LEFT;
			delayTick=0;
		}
		else if(event == EVENT_OBSTACLE_RIGHT){
			state = STATE_OBSTACLE_RIGHT;
			delayTick=0;
		}
		else if(event == EVENT_MINE_DETECTED){
			state = STATE_MINE_DETECTED;
			delayTick=0;
		}
		else{
		// Do nothing
		}
	}
	// Idle the motors in low power mode
	if(state == STATE_STOP){
		HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_3);
		HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_4);
	}
	//Short acceleration to speed up the motors
	else if(state == STATE_ACCELERATE){
		if(delayTick<=2){
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 100);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, motorSpeed-20);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 100);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, motorSpeed-25);
		HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);
		HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_3);
		HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_4);
		}
		else{
			state = STATE_FORWARD;
		}
	}
	//Move both motors forward
	else if(state == STATE_FORWARD){
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 100);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, motorSpeed+14);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 100);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, motorSpeed);
		HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);
		HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_3);
		HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_4);
	}
	//Reverse both motors for a set delay
	else if(state == STATE_OBSTACLE_LEFT){
		if(delayTick<30){
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, motorSpeed+6);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 100);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, motorSpeed-1);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 100);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_3);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_4);
		}
		//Stop both motor for a set delay
		else if(delayTick>=30 && delayTick<=50){
			HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_1); //Stop right motor
			HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_2);
			HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_3); //Stop left motor
			HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_4);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, motorSpeed);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 100);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 100);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, motorSpeed);
		}
		//Reverse one motor for a set delay (turn robot)
		else if(delayTick>=50){
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_3);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_4);
			//Stop both motors before changing state to accelerate
			if(delayTick>=65){
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_1);
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_2);
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_3);
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_4);
				state = STATE_ACCELERATE;
				delayTick=0;
				colorSensor_enable = 1;
				distSensor_Enable=1;
				buttonRight = 0;
				buttonLeft = 0;
			}
		}
	}
	else if(state == STATE_OBSTACLE_RIGHT){
		//Reverse both motors for a set delay
		if(delayTick<30){
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, motorSpeed+6);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 100);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, motorSpeed-1);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 100);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_3);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_4);
		}
		//Stop both motor for a set delay
		else if(delayTick>=30 && delayTick<=50){
			HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_2);
			HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_4);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 100);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, motorSpeed);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, motorSpeed);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 100);
		}
		//Reverse one motor for a set delay (turn robot)
		else if(delayTick>=50){
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_3);
			HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_4);
			//Stop both motors before changing state to accelerate
			if(delayTick>=65){
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_1);
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_2);
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_3);
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_4);
				state = STATE_ACCELERATE;
				delayTick=0;
				colorSensor_enable = 1;
				distSensor_Enable=1;
				buttonRight = 0;
				buttonLeft = 0;
			}
		}
	}
	//Stops the robot for a set delay if a mine is found
	else if(state == STATE_MINE_DETECTED){
		HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_3);
		HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_4);
		//Change state to obstacle_right or obstacle_left each other time
		if(delayTick>=50){
			direction *= -1;
			if(direction==1){
				state= STATE_OBSTACLE_RIGHT;
			}
			else if(direction==-1){
				state= STATE_OBSTACLE_LEFT;
			}
			delayTick=0;
			enableBuzzer=0;
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_3);
			BLE_sendData(8);
		}
	}
}

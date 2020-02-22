
#include "main.h"
#include "stm32f4xx_hal.h"
#include "motor_control.h"
#include "BLE_sendData.h"
#include "event_queue.h"

extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart6;
extern myQueue *pmotorQueue;
extern int distSensor_Enable;
extern int colorSensor_enable;

int rising = 1;
int rising_2 = 1;
int distCount = 0;
int dist2Count = 0;

int buttonRight,buttonLeft;

char strr[10];

uint32_t time_start;
uint32_t time_end;
uint32_t time_start2;
uint32_t time_end_2;
uint32_t Distance = 100;
uint32_t Distance_2 = 100;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == ECHO_HCSR04_Pin){
		if(rising == 0){
			time_end= __HAL_TIM_GET_COUNTER(&htim4);
			Distance = ((time_end-time_start)/42);
			rising = 1;
		}
		else if(rising == 1){
			__HAL_TIM_SET_COUNTER(&htim4,0);
			time_start=__HAL_TIM_GET_COUNTER(&htim4);
			rising = 0;
		}
	}
    if(GPIO_Pin == ECHO_HCSR04_2_Pin){
		if(rising_2 == 0){
			time_end_2 = __HAL_TIM_GET_COUNTER(&htim5);
         	Distance_2 = ((time_end_2-time_start2)/42);
			rising_2 = 1;
		}
		else if(rising_2 == 1){
			 __HAL_TIM_SET_COUNTER(&htim5,0);
			time_start2 = __HAL_TIM_GET_COUNTER(&htim5);
			rising_2 = 0;
		}
	}
	else if(GPIO_Pin == buttonLeft_Pin){
		buttonLeft=1;
	}
	else if(GPIO_Pin == buttonRight_Pin){
		buttonRight=1;
	}
}

/* Funtion to check the distance of both ultrasonic sensors.
 * After a set count or if a button is pressed, write the correct event in the motor queue */
void checkDist(void){
	if(distSensor_Enable==1){
		if(Distance<=40){
			distCount ++;
		}
		else{
			distCount=0;
		}
		if(Distance_2<=40){
			dist2Count ++;
		}
		else{
			dist2Count=0;
		}
		if((distCount>3)||(buttonRight == 1)){
			distSensor_Enable=0;
			BLE_sendData(2);
			writeQueue(pmotorQueue, EVENT_OBSTACLE_RIGHT);
			dist2Count=0;
			distCount=0;
		}
		if((dist2Count>3)||(buttonLeft==1)){
			distSensor_Enable=0;
			BLE_sendData(2);
			writeQueue(pmotorQueue, EVENT_OBSTACLE_LEFT);
			distCount=0;
			dist2Count=0;
		}
	}
}

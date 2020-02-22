

#include "main.h"
#include "servoArm.h"
#include "stm32f4xx_hal.h"
#include "mineSearching.h"

extern int calibrationDone;
extern TIM_HandleTypeDef htim10;

int servoState=0;
int Direction=1;

/* Changes the state depending on the Direction. Makes the servo move left or right. */
void changeState(){
	if(Direction==1){
		servoState++;
	}
	else{
		servoState--;
	}
}

/* Function used to calibrate the ground of the current servoState while calibrationDone is 0.
 * Else check ground for mines. */
void calibrateCheckground(){
	if(calibrationDone==0){
		startCalibration(servoState);
	}
	else{
		checkGround(servoState);
	}
	changeState();
}

/* Statemachine to change the PWM pulse of tim10 channel1 which moves the position of the servo. */
void servoStateMachine(){
	switch(servoState) {
	case 0:
		Direction=1;
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 3846);
		calibrateCheckground();
	    break;

	case 1:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 4246);
		calibrateCheckground();
	    break;

	case 2:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 4646);
		calibrateCheckground();
		break;

	case 3:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 5046);
		calibrateCheckground();
		break;

	case 4:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 5446);
		calibrateCheckground();
		break;

	case 5:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 5846);
		calibrateCheckground();
		break;

	case 6:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 6246);
		calibrateCheckground();
		break;

	case 7:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 6646);
		calibrateCheckground();
		break;

	case 8:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 7046);
		calibrateCheckground();
		break;

	case 9:
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 7446);
		calibrateCheckground();
		break;

	case 10:
		Direction=0;
		__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 7846);
		calibrateCheckground();
		break;

	default :
	   break;
	}
}

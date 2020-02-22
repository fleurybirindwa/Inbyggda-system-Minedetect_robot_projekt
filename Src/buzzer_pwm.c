
#include "buzzer_pwm.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "mineSearching.h"

extern TIM_HandleTypeDef htim2;
extern int buzzTick;
extern int enableBuzzer;

int ledBlink=0;
int buzzTemp=0;

/*--------------------------------------------------------------------------------------------------------------
BuzzPWM(PWM,autoR)
Function to change the autoreload and PWM pulse of htim channel_3
 --------------------------------------------------------------------------------------------------------------*/
void buzzPWM(int PWM,int autoR){
	__HAL_TIM_SET_COUNTER(&htim2,0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, autoR);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3, PWM);
}

/* -----------------------------------------------------------------------------------------------------------
 start_Buzzer()
 Function to change pitch of the buzzer. Using a tick the BuzzPWM function is called with different values.
 The RGB-LED color is set to blink the led with the colors red and blue together with the buzzer.
 ------------------------------------------------------------------------------------------------------------*/
void start_Buzzer(){
	if(enableBuzzer==1){
		if(buzzTick>=10){
			buzzTick=0;
			if(buzzTemp==0){
				if(ledBlink==0){
					buzzPWM(2500,20999);
				}
				else{
					setColorRGB(0,0,220,30,30);
					buzzPWM(2500,20999);
				}
				buzzTemp=1;
			}
			else if(buzzTemp==1){
				if(ledBlink==0){
					buzzPWM(3500,30000);
				}
				else{
					setColorRGB(0,0,30,30,220);
					buzzPWM(3500,30000);
				}
				buzzTemp=0;
			}
		}
	}
}

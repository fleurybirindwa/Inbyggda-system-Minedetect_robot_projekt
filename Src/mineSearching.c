/*
 * mineSearching.c

 */

#include "mineSearching.h"
#include "motor_control.h"
#include "event_queue.h"
#include "colorSensor_I2C.h"
#include "BLE_sendData.h"
#include "BLE_ReceiveData.h"
#include "main.h"

extern myQueue *pmotorQueue;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern int distSensor_Enable;
extern int colorSensor_enable;
extern int enableBuzzer;

int countColor = 0;
int clear,red,green,blue;
int calibratedValues=0;
int calibrationDone=0;

float rgbLedRed[]={610,811,1153,1753,1983,2143,2248,2299,2232,1913,1327};
float rgbLedGreen[]={704,946,1343,2175,2537,2805,2987,3082,2949,2407,1595};
float rgbLedBlue[]={601,837,1215,1921,2237,2470,2628,2711,2597,2124,1411};

float colorMaxRed[11];
float colorMinRed[]={10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000};
float colorMaxGreen[11];
float colorMinGreen[]={10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000};
float colorMaxBlue[11];
float colorMinBlue[]={10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000};
float colorMaxClear[11];
float colorMinClear[]={10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000};

float redmed[11],greenmed[11],bluemed[11],clearmed[11];
float redMinLimit[11],greenMinLimit[11],blueMinLimit[11],clearMinLimit[11];
float redMaxLimit[11],greenMaxLimit[11],blueMaxLimit[11],clearMaxLimit[11];
/*-------------------------------------------------------------------------------------------
startCalibration(servoState)

keeps track of if the robot is in the calibration stage and if it is isnt reactiong to
mine targets. When it is calibrateds with 100 values it turn on the mine searching.

it has servoState as a input, servoState is what position the servo have and the calibration is
done on each position.
-------------------------------------------------------------------------------------------*/

void startCalibration(int servoState){
	if(calibratedValues<=150){     // get values for calibrate first 100 values
		calibrateColor(servoState);        //calibrating function
		colorSensor_enable=0;
		calibratedValues = calibratedValues+1 ;
		if(calibratedValues>150){
			colorSensor_enable=1;// "start" the color sensor
			distSensor_Enable=1;
			calibrationDone=1;
			BLE_sendData(3);
		}
	}
}


/*-------------------------------------------------------------------------------------------
getRGBValues()

Gets the rgb value from the i2c readregister function.
-------------------------------------------------------------------------------------------*/
void getRGBValue(){
	clear = colorSensor_ReadRegisters(0x14);
	red   = colorSensor_ReadRegisters(0x16);
	green = colorSensor_ReadRegisters(0x18);
	blue  = colorSensor_ReadRegisters(0x1A);
}

/*-------------------------------------------------------------------------------------------
checGround(servoState)

It has servoState (current servo position) as an input.
The function compares the current rgb value whit the calibration values calculated in  calibrateColor(servoState) downbelow
and if it is outside the calubration range it adding on a flag and if it have been outside this range 3 times a mine is found.
if the value is inside the range the values is send to the calibrate function.
-------------------------------------------------------------------------------------------*/
int checkGround(int servoState){
	getRGBValue();
	if(colorSensor_enable==1){
		if((red>(redMinLimit[servoState]*redmed[servoState]-15))&&(red<(redMaxLimit[servoState]*redmed[servoState]+25))
			&&(green>(greenMinLimit[servoState]*greenmed[servoState]-15))&&(green<(greenMaxLimit[servoState]*greenmed[servoState]+25))
			&&(blue>(blueMinLimit[servoState]*bluemed[servoState]-15))&&(blue<(blueMaxLimit[servoState]*bluemed[servoState]+25))){
			countColor=0;
			calibrateColor(servoState);
		}
		else{
			countColor++;
		}
		if(countColor>2){
			mineFound(servoState);
			countColor=0;
			return 1;
		}
	}
	return 0;
}
/*-------------------------------------------------------------------------------------------
mineFound(servoState)

if a mine is found this function is turning the minesearching of,sends that a mine is found to the motorqueue,
turning the led on in the color of the mine that is found, starts the buzzer and sends minefound to the app.
-------------------------------------------------------------------------------------------*/
void mineFound(int servoState){
	writeQueue(pmotorQueue, EVENT_MINE_DETECTED); //tell the motors a mine is found.
	colorSensor_enable=0;
	distSensor_Enable=0;
	enableBuzzer=1;
	setColorRGB(1,servoState,red,green,blue);
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3);
	BLE_sendData(1);
}

/*-------------------------------------------------------------------------------------------
calibrateColor(servoState)

Saves max and min value of the rgb channels.
calculates the average value of each rgb channel.
calculate how big part min and max value is of the average and usea it as a limit when comparing the floor value to the calibrate value.

Each servoState(servo position) have its own calibration values.
-------------------------------------------------------------------------------------------*/
void calibrateColor(int servoState){
	getRGBValue();
	if(colorMinRed[servoState]>red)
		colorMinRed[servoState] = red;

	if(colorMaxRed[servoState]<red)
		colorMaxRed[servoState] = red;

	if(colorMinGreen[servoState]>green)
		colorMinGreen[servoState] = green;

	if(colorMaxGreen[servoState]<green)
		colorMaxGreen[servoState] = green;

	if(colorMinBlue[servoState]>blue)
		colorMinBlue[servoState] = blue;

	if(colorMaxBlue[servoState]<blue)
		colorMaxBlue[servoState] = blue;

	if(colorMinClear[servoState]>clear)
		colorMinClear[servoState] = clear;

	if(colorMaxClear[servoState]<clear)
		colorMaxClear[servoState] = clear;

	redmed[servoState] = 0.95*redmed[servoState]+0.05*red;
	greenmed[servoState] = 0.95*greenmed[servoState]+0.05*green;
	bluemed[servoState] = 0.95*bluemed[servoState]+0.05*blue;
	clearmed[servoState] = 0.95*clearmed[servoState]+0.05*clear;

	redMinLimit[servoState]   = colorMinRed[servoState]/redmed[servoState];
	greenMinLimit[servoState] = colorMinGreen[servoState]/greenmed[servoState];
	blueMinLimit[servoState]  = colorMinBlue[servoState]/bluemed[servoState];
	clearMinLimit[servoState] = colorMinClear[servoState]/clearmed[servoState];

	redMaxLimit[servoState]   = colorMaxRed[servoState]/redmed[servoState];
	greenMaxLimit[servoState] = colorMaxGreen[servoState]/greenmed[servoState];
	blueMaxLimit[servoState]  = colorMaxBlue[servoState]/bluemed[servoState];
	clearMaxLimit[servoState] = colorMaxClear[servoState]/clearmed[servoState];
}

/*-------------------------------------------------------------------------------------------
 setColorRGB(servoState)

 lights the rgb led. if it is trigged by a mine a calculation is done to make it 0-255
-------------------------------------------------------------------------------------------*/

void setColorRGB(int mine ,int servoState,float rgbRed, float rgbGreen,float rgbBlue){
	if(mine==1){
		htim1.Instance->CCR1 = 255-((rgbRed/rgbLedRed[servoState])*255);
		htim1.Instance->CCR2 = 255-((rgbGreen/rgbLedGreen[servoState])*255);
		htim1.Instance->CCR3 = 255-((rgbBlue/rgbLedBlue[servoState])*255);
	}
	else{
		htim1.Instance->CCR1 = 255-rgbRed;
		htim1.Instance->CCR2 = 255-rgbGreen;
		htim1.Instance->CCR3 = 255-rgbBlue;
	}
}



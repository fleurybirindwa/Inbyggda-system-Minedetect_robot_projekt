

#include "event_queue.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "motor_control.h"

//Functions for init,write and read a event queue.
void initQueue(myQueue *Q){
	Q->readIndex=0;
	Q->writeIndex=0;
	Q->numOfElements=0;
}

//Function to write to the queue
void writeQueue(myQueue *Q,event_e event){
	if(Q->numOfElements>20){
		return;
	}
	else{
		__disable_irq();
		Q->eventBuffer[Q->writeIndex]=event;
		Q->writeIndex=(Q->writeIndex+1)%buffSize;
		Q->numOfElements++;
		__enable_irq();
	}
}

//Function to read from the queue
event_e readQueue(myQueue *Q){
	if(Q->numOfElements==0){
		return EVENT_EMPTY;
	}
	else{
		__disable_irq();
		event_e e;
		e = Q->eventBuffer[Q->readIndex];
		Q->readIndex=(Q->readIndex+1)%buffSize;
		Q->numOfElements--;
		__enable_irq();
		return e;
	}
}


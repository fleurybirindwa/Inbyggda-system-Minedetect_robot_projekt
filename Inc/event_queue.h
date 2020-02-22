

#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

#include "motor_control.h"

void stateMachine(myQueue*);
void initQueue(myQueue*);
void writeQueue(myQueue*,event_e);
event_e readQueue(myQueue*);

#endif /* EVENT_QUEUE_H_ */

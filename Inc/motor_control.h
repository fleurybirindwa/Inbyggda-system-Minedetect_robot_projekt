
#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_
#define buffSize 20

typedef enum state{
	STATE_STOP,
	STATE_FORWARD,
	STATE_REVERSE,
	STATE_OBSTACLE_LEFT,
	STATE_OBSTACLE_RIGHT,
	STATE_MINE_DETECTED,
	STATE_ACCELERATE
} motorState_t;

typedef enum{
	EVENT_EMPTY,
	EVENT_MINEDETECT,
	EVENT_START,
	EVENT_STOP,
	EVENT_OBSTACLE_LEFT,
	EVENT_OBSTACLE_RIGHT,
	EVENT_MINE_DETECTED
} event_e;

typedef struct{
	event_e eventBuffer[buffSize];
	int readIndex;
	int writeIndex;
	int numOfElements;
} myQueue;

typedef enum{
	STATE_IDLE,
	STATE_ONE,
} state_e;

void motorStateMachine(myQueue*);

#endif /* MOTOR_CONTROL_H_ */


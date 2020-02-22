
#ifndef BLE_RECEIVEDATA_H_
#define BLE_RECEIVEDATA_H_

typedef enum {
	STATE_BLE_IDLE,
	STATE_BLE_START,
	STATE_BLE_STOP,
	STATE_BLE_BUZZER
} BLE_state_e;

void BLE_Statemachine();

#endif /* BLE_RECEIVEDATA_H_ */



#ifndef POZYX_I2C_H_
#define POZYX_I2C_H_

typedef enum{
	STATE_POZ_IDLE,
	STATE_POZ_ONE
} state_pozyx;

void Pozyx_ReadRegisters();
void pozyx_Init();

#endif /* POZYX_I2C_H_ */

/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#include <stdint.h>

#ifndef MOTOR_H
#define MOTOR_H

typedef enum
{
	MOT_DC,
	MOT_BLDC
} _MOT_motorType;

typedef enum
{
	MOT_NEG,
	MOT_POS
} _MOT_motorDirection;

void MOT_defineMotorType(_MOT_motorType motorType);

void MOT_initMotor(void);
void MOT_startMotor(void);
void MOT_stopMotor(void);
void MOT_commandDutyCycle(uint16_t dutyCycle);
void MOT_commandDirection(_MOT_motorDirection direction);

#endif

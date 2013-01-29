/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#ifndef MOTOR_BLDC_H
#define MOTOR_BLDC_H

/* Standard or provided libs */
#include <stdbool.h>
#include <stdint.h>


#define BLDC_DEFAULT_PWM_FREQ		16000
#define BLDC_MIN_DUTY_CYCLE			10000

// Use these to keep track of the
//	current state of the motor
//	(this is a state machine)
typedef enum
{
	BLDC_LOCKED,
	BLDC_STOPPED,
	BLDC_STARTING,
	BLDC_RUNNING
} _BLDC_motorState;

typedef enum
{
	BLDC_NEG,
	BLDC_POS
} _BLDC_motorDirection;

typedef enum
{
	BLDC_SENSORLESS,
	BLDC_HALL
} _BLDC_sensor;


// These are the motor interface functions,
//	or the "public" functions
void BLDC_initMotor(void);
void BLDC_startMotor(void);
void BLDC_stopMotor(void);
void BLDC_commandDutyCycle(uint16_t dutyCycle);
void BLDC_commandDirection(bool direction);

uint8_t BLDC_getMotorState(void);

#endif

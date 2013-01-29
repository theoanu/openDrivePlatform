/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#ifndef MOTORDC_H
#define MOTORDC_H

/* Standard or provided libs */
#include <stdint.h>

#define MDC_DEFAULT_PWM_FREQ		16000
#define MDC_MIN_DUTY_CYCLE			10000

// Use these to keep track of the
//	current state of the motor
//	(this is a state machine)
typedef enum
{
	MDC_STOPPED,
	MDC_RUNNING
} _MDC_motorState;

typedef enum
{
	MDC_NEG,
	MDC_POS
} _MDC_motorDirection;

// These are the motor interface functions,
//	or the "public" functions
void MDC_initMotor(void);
void MDC_startMotor(void);
void MDC_stopMotor(void);
void MDC_commandDutyCycle(uint16_t dutyCycle);
void MDC_commandDirection(_MDC_motorDirection direction);

uint8_t MDC_getMotorState(void);

#endif

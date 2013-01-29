/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#include "stm32f10x_adc.h"
#include "misc.h"
#include "motorDc.h"
#include "mpwm.h"
#include "gpio.h"

/* Global variables */
typedef struct{
	_MDC_motorState state;
	uint16_t dutyCycle;
	_MDC_motorDirection direction;

	uint8_t sensorState;

	uint8_t hallToSector[8];
} _motor;

typedef struct{
	_MDC_motorDirection direction;
	uint16_t dutyCycle;
}_motor_command;

_motor MDC_motor;
_motor_command MDC_command;

/***************************************************************
 * Function:	void MDC_initMotor(void)
 *
 * Purpose:		This function is called by higher-level software in order
 * 					to initialize the motor in preparation for operation.
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	none
 **************************************************************/
void
MDC_initMotor(void)
{
	MPWM_initMotorPwm();
	MPWM_setMotorPwmFreq(16000);

	MDC_stopMotor();
	MDC_commandDirection(MDC_POS);

	return;
} // END MDC_initMotor()

/***************************************************************
 * Function:	void MDC_startMotor(void)
 *
 * Purpose:		This function is called by higher-level software
 * 					when motor rotation should begin operating
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	MDC_motor.sector, motor.state
 **************************************************************/
void
MDC_startMotor(void)
{
	// Only allow this routine to execute if
	//	the motor is in the STOPPED state
	if(MDC_motor.state == MDC_STOPPED){
		MDC_motor.state = MDC_RUNNING;
	}

	return;
} // END MDC_startMotor()

/***************************************************************
 * Function:	void MDC_stopMotor(void)
 *
 * Purpose:		This function is called by higher-level software
 * 					when motor rotation should cease
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	MDC_motor.state
 **************************************************************/
void
MDC_stopMotor(void)
{
	// Place each phase in the DORMANT state
	MPWM_setPhaseDutyCycle(MPWM_PH_A, MPWM_DORMANT, 0);
	MPWM_setPhaseDutyCycle(MPWM_PH_B, MPWM_DORMANT, 0);
	MPWM_setPhaseDutyCycle(MPWM_PH_C, MPWM_DORMANT, 0);

	// Place the motor in the STOPPED state
	MDC_motor.state = MDC_STOPPED;

	return;
} // END MDC_stopMotor()

/***************************************************************
 * Function:	void MDC_commandDutyCycle(unsigned int dutyCycle);
 *
 * Purpose:		This function is called by higher-level software
 * 					to modify the duty cycle.
 *
 * Parameters:	uint16_t dutyCycle		This is the fixed-point representation
 * 										of the motor duty cycle.  0%-100% is scaled
 * 										to 0-65535
 *
 * Returns:		none
 *
 * Globals affected:	MDC_motor.dutyCycle
 **************************************************************/
void
MDC_commandDutyCycle(uint16_t dutyCycle)
{
	// No ramp implemented
	MDC_motor.dutyCycle = dutyCycle;
	MDC_motor.direction = MDC_command.direction;

	// Calculate the high side and low side duty cycles
	uint16_t halfDutyCycle = (MDC_motor.dutyCycle >> 1);
	uint16_t highSideDutyCycle = 32767 + halfDutyCycle;
	uint16_t lowSideDutyCycle = 32767 - halfDutyCycle;

	// Based on the motor direction, determine which phase is to be high
	//	and which phase is to be low
	if(MDC_motor.direction == MDC_POS)
	{
		MPWM_setPhaseDutyCycle(MPWM_PH_A, MPWM_HI_STATE, highSideDutyCycle);
		MPWM_setPhaseDutyCycle(MPWM_PH_B, MPWM_HI_STATE, lowSideDutyCycle);
	}else{
		MPWM_setPhaseDutyCycle(MPWM_PH_A, MPWM_HI_STATE, lowSideDutyCycle);
		MPWM_setPhaseDutyCycle(MPWM_PH_B, MPWM_HI_STATE, highSideDutyCycle);
	}

	MPWM_setAdcSamplingTime(32767);

	return;
}

/***************************************************************
 * Function:	void MDC_commandDirection(_MDC_motorDirection direction);
 *
 * Purpose:		This function is called by higher-level software
 * 					to modify the motor direction.
 *
 * Parameters:	bool direction	This determines the direction of the motor.  Valid
 * 								values are MDC_POS and MDC_NEG.
 *
 * Returns:		none
 *
 * Globals affected:	MDC_motor.direction
 **************************************************************/
void
MDC_commandDirection(_MDC_motorDirection direction)
{
	MDC_command.direction = direction;
	return;
}

/***************************************************************
 * Function:	uint8_t MDC_getMotorState(void)
 *
 * Purpose:		This function is called by higher-level software
 * 					to retrieve the current motor state.
 *
 * Parameters:	none
 *
 * Returns:		uint8_t motor.state
 *
 * Globals affected:	none
 **************************************************************/
uint8_t
MDC_getMotorState(void){
	return MDC_motor.state;
}



/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/

#include "motor.h"
#include "motorBldc.h"
#include "motorDc.h"

/* Global variables */
typedef struct
{
	_MOT_motorType type;
} _motor;

_motor motor;

/***************************************************************
 * Function:	void MOT_defineMotorType(_MOT_motorType motorType)
 *
 * Purpose:		This defines the motor type to be used, which chooses
 * 					the motor library to be utilized
 *
 * Parameters:	_MOT_motorType motorType
 *
 * Returns:		none
 *
 * Globals affected:	motor.type
 **************************************************************/
void
MOT_defineMotorType(_MOT_motorType motorType)
{
	// Stop all motor activity
	BLDC_stopMotor();
	MDC_stopMotor();

	// Change the motor type
	motor.type = motorType;

	MOT_initMotor();

	return;
} // END MOT_defineMotorType()

/***************************************************************
 * Function:	void MOT_initMotor(void)
 *
 * Purpose:		To initialize the appropriate motor based on the
 * 					previously defined motor type
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	none
 **************************************************************/
void
MOT_initMotor(void)
{
	switch(motor.type)
	{
		case MOT_DC:
			MDC_initMotor();
			break;

		case MOT_BLDC:
			BLDC_initMotor();
			break;

		default:
			BLDC_initMotor();
			break;
	}

	return;
} // END MOT_initMotor()

/***************************************************************
 * Function:	void MOT_startMotor(void)
 *
 * Purpose:		To start the appropriate motor based on the
 * 					previously defined motor type
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	none
 **************************************************************/
void
MOT_startMotor(void)
{
	switch(motor.type)
	{
		case MOT_DC:
			MDC_startMotor();
			break;

		case MOT_BLDC:
			BLDC_startMotor();
			break;

		default:
			BLDC_startMotor();
			break;
	}

	return;
} // END MOT_startMotor()

/***************************************************************
 * Function:	void MOT_stopMotor(void)
 *
 * Purpose:		To stop the appropriate motor based on the
 * 					previously defined motor type
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	none
 **************************************************************/
void
MOT_stopMotor(void)
{
	switch(motor.type)
	{
		case MOT_DC:
			MDC_stopMotor();
			break;

		case MOT_BLDC:
			BLDC_stopMotor();
			break;

		default:
			BLDC_stopMotor();
			break;
	}

	return;
} // END MOT_stopMotor()

/***************************************************************
 * Function:	void MOT_commandDutyCycle(uint16_t dutyCycle)
 *
 * Purpose:		To update the duty cycle required by higher-
 * 					level software
 *
 * Parameters:	uint16_t dutyCycle		This is the fixed-point representation
 * 										of the motor duty cycle.  0%-100% is scaled
 * 										to 0-65535
 *
 * Returns:		none
 *
 * Globals affected:	none
 **************************************************************/
void
MOT_commandDutyCycle(uint16_t dutyCycle)
{
	if(dutyCycle < MOT_MIN_DUTY_CYCLE)
	{
		dutyCycle = 0;
	}

	switch(motor.type)
	{
		case MOT_DC:
			MDC_commandDutyCycle(dutyCycle);
			break;

		case MOT_BLDC:
			BLDC_commandDutyCycle(dutyCycle);
			break;

		default:
			BLDC_stopMotor();
			break;
	}

	return;
} // END MOT_commandDutyCycle

/***************************************************************
 * Function:	void MOT_commandDirection(_MOT_motorDirection direction)
 *
 * Purpose:		To load the appropriate motor direction based on
 * 					the desired direction and on the motor type
 * 					chose previously
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	none
 **************************************************************/
void
MOT_commandDirection(_MOT_motorDirection direction)
{
	switch(motor.type)
	{
		case MOT_DC:
			MDC_commandDirection(direction);
			break;

		case MOT_BLDC:
			BLDC_commandDirection(direction);
			break;

		default:
			BLDC_commandDirection(direction);
			break;
	}

	return;
} // END MOT_commandDirection()


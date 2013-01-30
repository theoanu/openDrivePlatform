/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#include "gpio.h"
#include "osc.h"
#include "milliSecTimer.h"
#include "motor.h"
#include "rcPwm.h"
#include "adc.h"

void initDio(void);

int
main(void)
{
	// Initialize oscillator
	OSC_initClock();

	// Initialize the milliSecond timer
	MSTMR_initMilliSecTimer();

	// Initialize timing variables
	uint32_t now = MSTMR_getMilliSeconds();
	uint32_t lastExecutionTime = now;

	// Initialize General-purpose I/O (when code is more complete,
	//	this will be done in the individual software modules
	initDio();

	// Initialize ADC
	ADC_initAdc();

	// Initialize RC PWM module
	RCPWM_initRcPwm();

	// Initialize motor
	MOT_defineMotorType(MOT_BLDC);

	// Initialize UART/CLI

	// Initialize bootloader

	// Infinite loop
    while(1)
    {
    	// Use the millisecond timer to time all operations within the main loop
    	now = MSTMR_getMilliSeconds();

    	// Error checking to ensure that the lastExecutionTime
    	//	is not somehow much greater than the now variable
    	//	through some unforeseen event
    	if(lastExecutionTime > now)
    	{
    		lastExecutionTime = now;
    	}

    	// Everything in this IF statement will be executed one time every millisecond
    	if(now > lastExecutionTime)
    	{
    		lastExecutionTime = now;

         	// Get requested duty cycle from rcPwm/USB/UART/I2C
    		uint32_t speedDemand = RCPWM_getSpeedDemand();
    		if(speedDemand > 15000)
    			speedDemand = 15000;

        	// Pass requested duty cycle to the motor
    		MOT_commandDirection(MOT_POS);
    		MOT_commandDutyCycle(speedDemand);
    	} // END if statement
    } //END while loop

    return 0;
}

void
initDio(void)
{
	// Setup analog input pins
	// Analog input pins are not remapped
	GPIO_pinSetup(GPIO_PORT_A, 0, GPIO_INPUT_ANALOG);	// PHA FBK
	GPIO_pinSetup(GPIO_PORT_A, 1, GPIO_INPUT_ANALOG);	// PHB FBK
	GPIO_pinSetup(GPIO_PORT_A, 2, GPIO_INPUT_ANALOG);	// PHC FBK
	GPIO_pinSetup(GPIO_PORT_A, 3, GPIO_INPUT_ANALOG);	// Bus Voltage
	GPIO_pinSetup(GPIO_PORT_A, 7, GPIO_INPUT_ANALOG);	// Bus Current
	GPIO_pinSetup(GPIO_PORT_A, 4, GPIO_INPUT_ANALOG);	// Speed control

	// Setup output tach pin
	GPIO_pinSetup(GPIO_PORT_A, 5, GPIO_OUTPUT_PP);

	// Setup hall sensors
	GPIO_pinSetup(GPIO_PORT_B, 0, GPIO_FLOATING_INPUT);
	GPIO_pinSetup(GPIO_PORT_B, 1, GPIO_FLOATING_INPUT);
	GPIO_pinSetup(GPIO_PORT_B, 2, GPIO_FLOATING_INPUT);

	// Setup current fault as a pull-up to eliminate external pull-ups
	// Overcurrent sensing pin is not remapped
	GPIO_pinSetup(GPIO_PORT_A, 7, GPIO_INPUT_PU_OR_PD);	// Overcurrent

	return;
} // END initDio()

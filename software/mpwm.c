/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
/* Standard or provided libs */
#include "stm32f10x_tim.h"
#include "stm32f10x_dbgmcu.h"

/* User-generated libs */
#include "osc.h"
#include "mpwm.h"
#include "gpio.h"
#include "adc.h"

typedef struct{
	_phaseState stateA, stateB, stateC;
} _phase;

_phase MPWM_motorPhase;

/*
 * Private function declarations
 */
void MPWM_setDeadTime(float deadTimeInUs);


/*
 * Implementations
 */

/***************************************************************************
 * 	Function:	void MPWM_initMotorPwm(void);
 *
 * 	Purpose:	To initialize the advanced timer as a complimentary 3-phase
 * 					motor PWM module
 *
 * 	Parameters:	none
 ***************************************************************************/
void
MPWM_initMotorPwm(void)
{
    // Setup TIM1 outputs
	// TIM1 pins are not remapped
	GPIO_pinSetup(GPIO_PORT_B, 13, GPIO_OUTPUT_ALT_PP);
	GPIO_pinSetup(GPIO_PORT_B, 14, GPIO_OUTPUT_ALT_PP);
	GPIO_pinSetup(GPIO_PORT_B, 15, GPIO_OUTPUT_ALT_PP);
	GPIO_pinSetup(GPIO_PORT_A, 8, GPIO_OUTPUT_ALT_PP);
	GPIO_pinSetup(GPIO_PORT_A, 9, GPIO_OUTPUT_ALT_PP);
	GPIO_pinSetup(GPIO_PORT_A, 10, GPIO_OUTPUT_ALT_PP);

	DBGMCU->CR |= (uint32_t)(1 << 10);		// Stop TMR1 when in debug mode

	// Enables interrupt in NVIC
	NVIC_EnableIRQ(TIM1_CC_IRQn);

	// Clock division: tDTS = 1 * tCK_INT
	TIM1->CR1 |= (uint16_t)(0b00 << 8);

	// OC4REF signal is used as trigger output (TRGO)
	TIM1->CR2 |= (uint16_t)(0b111 << 4);

	// Interrupt on CC1, CC2, and CC3
	TIM1->DIER = (uint16_t)(0b1 << 4);

	MPWM_setMotorPwmFreq(20000);

	// Place each phase in the DORMANT state
	MPWM_setPhaseDutyCycle(MPWM_PH_A, MPWM_DORMANT, 0);
	MPWM_setPhaseDutyCycle(MPWM_PH_B, MPWM_DORMANT, 0);
	MPWM_setPhaseDutyCycle(MPWM_PH_C, MPWM_DORMANT, 0);

	// Dead-time generation 1us dead-time
	MPWM_setDeadTime(1.0);

	// Set ADC sampling time
	MPWM_setAdcSamplingTime(64000);

	// Master Output Enable on
	TIM1->BDTR |= (uint16_t)(1 << 15);

	// Set up channel 4 as a timer only
	//	capture/compare.  Load the CCR4 register
	//	every time the duty cycle is updated.
	//	Use the capture/compare event as the
	//	ADC trigger. *** commented out b/c these
	//	are the values at reset anyway ***
	TIM1->CCMR2 |= (uint16_t)((0b0 << 15)	// OC4CE
							+ (0b000 << 12)	// OC4M
							+ (0b0 << 11)	// OC4PE
							+ (0b0 << 10)	// OC4FE
							+ (0b00 << 8)); // CC4S

	TIM1->CCER |= (uint16_t)((0b1 << 13)	// Change the polarity CH4 (ADC triggers on falling edge)
							+ (0b0 << 12)); // Enable the output of CH4

	// Counter enabled
	TIM1->CR1 |= (uint16_t)(0b1 << 0);

	return;
} // END MPWM_initMotorPwm()

/***************************************************************************
 * 	Function:	void MPWM_setMotorPwmFreq(uint16_t pwmFrequency);
 *
 * 	Purpose:	To easily set the pwm frequency for the motor PWM
 *
 * 	Parameters:	uint16_t pwmFrequency	Valid values are 1200 to 65535, which
 * 										determine the pwm frequency in hertz
 ***************************************************************************/
void
MPWM_setMotorPwmFreq(uint16_t pwmFrequency)
{
	// Limit PWM frequency to lower values (in Hz).
	//	An upper limit is not necessary since the
	//	value is intrinsically limited by the width
	if(pwmFrequency < 1200)
		pwmFrequency = 1200;

	uint32_t timerOneFreq = OSC_getClockFreq();
	uint16_t arrValue = (uint16_t)(timerOneFreq/(uint32_t)pwmFrequency);

	TIM1->ARR = arrValue;

	return;
}

/***************************************************************************
 * Function:	void MPWM_setDeadTime(float deadTimeInUs);
 ***************************************************************************/

void
MPWM_setDeadTime(float deadTimeInUs)
{
	float deadTimeInCycles = (uint16_t)((deadTimeInUs/1000000) * (float)OSC_getClockFreq());

	// We can only use 7 bits in the dead-time generator register, so the value must be limited
	if(deadTimeInCycles > 127.0)
	{
		deadTimeInCycles = 127.0;
	}

	// Convert to a byte value
	uint16_t deadTimeGeneratorReg = (uint16_t)deadTimeInCycles;

	// Save the BDTR register
	uint16_t BDTR_reg = TIM1->BDTR;

	// Clear the bits of the dead time register
	BDTR_reg &= 0xff00;

	BDTR_reg |= deadTimeGeneratorReg;

	// Load the dead time register with the new value
	TIM1->BDTR = BDTR_reg;

	return;

} // END MPWM_setDeadTime()

/***************************************************************************
 * 	Function:	void MPWM_setAdcSamplingTime(uint16_t samplingTime);
 *
 * 	Purpose:	To change the time in the PWM cycle at which the ADC modules are triggered
 *
 * 	Parameters:	uint16_t samplingTime	Valid values range from 0 to 65535, which corresponds
 * 										to 0% - 100% of the PWM pulse.  If the value is set to
 * 										65000, that means that the ADC will be triggered near
 * 										the end of the PWM pulse, regardless of the duty cycle.
 *
 * 	Notes:		none
 *
 * 	Example:	none
 ***************************************************************************/
void
MPWM_setAdcSamplingTime(uint16_t samplingTime)
{
	uint16_t adcSampleTime = (uint16_t)(((uint32_t)samplingTime * (uint32_t)TIM1->ARR) >> 16);
	TIM1->CCR4 = adcSampleTime;		// Load the adc trigger register
}

/***************************************************************************
 * 	Function:	void MPWM_setPhaseDutyCycle(uint8_t phase, uint8_t state, uint16_t dutyCycle);
 *
 * 	Purpose:	To provide a clean and easy interface with which to interact with the phase
 * 					duty cycles.
 *
 * 	Parameters:	uint8_t phase 		Valid values are PH_A, PH_B, and PH_C.  This
 * 									parameter will determine which phase is being
 * 									modified when the function is called.
 * 				uint8_t state		Valid values are HI_STATE, LO_STATE, and DORMANT.
 * 									This parameter will determine the polarity of the
 * 									phase or, in the case of DORMANT, will render the
 * 									high-side and low-side FETs inactive
 * 				uint16_t dutyCycle	Valid values range from 0 to 65535, which corresponds
 * 									to 0% - 100% duty cycle
 *
 * 	Notes:		PWM is edge-aligned with a preset dead time of about 1us.  On a particular phase,
 * 				one FET is always active except during dead-time when HI_STATE or LO_STATE has been
 * 				entered as one of the parameters.  This can result in motor states which are highly
 * 				regenerative when the user is attempting to slow a motor down.
 *
 * 	Example:	The user wants to have current from phase B to phase A at an effective duty cycle
 * 				of 28.3%.  First, convert the number to the fixed-point domain: 28.3% of 65535 is 18546.
 * 				Then, call the functions to setup the appropriate outputs:
 * 				setPhaseDutyCycle(PH_C, DORMANT, 0);
 * 				setPhaseDutyCycle(PH_B, HI_STATE, 18546);
 * 				setPhaseDutyCycle(PH_A, LO_STATE, 18546);
 ***************************************************************************/

void
MPWM_setPhaseDutyCycle(uint8_t phase, uint8_t state, uint16_t dutyCycle)
{
	// Limit the duty cycle to a maximum so that the bootstrap capacitors
	//	on the FET drivers always have a chance to refresh their voltages.
	if(dutyCycle > 64000)
	{
		dutyCycle = 64000;
	}

	// The duty cycle is in unsigned 16-bit fractional number that
	//	needs to be translated into the 16-bit CCRx registers using
	//	fixed-point math.  Also, calculate the adc sample time as a
	//	percentage of the duty cycle.  The CCR4 will be used to
	//	specify the ADC sample time within the waveform.
	uint16_t dutyCycleRegValue = (uint16_t)(((uint32_t)dutyCycle * (uint32_t)TIM1->ARR) >> 16);

	if(phase == MPWM_PH_A)
	{
		// If the required state is HI_STATE, then the duty cycle should
		//	be applied with reference to the high-side switches (75% dc means
		//	75%dc on high-side)
		if(state == MPWM_HI_STATE)
		{
			// A check to ensure that the phase is not already in a high state
			//	will keep the software from re-clearing and re-loading registers
			//	that won't actually change
			if(MPWM_motorPhase.stateA != MPWM_HI_STATE)
			{
				TIM1->CCER |= (uint16_t)(0b0101 << 0);// TIM1 CH1 and CH1N on, active high

				TIM1->CCMR1 &= 0xff00;	// clear CC1 bits to default
				TIM1->CCMR1 |= (uint16_t)(0b01100000 << 0);	// pwm mode 1
				MPWM_motorPhase.stateA = MPWM_HI_STATE;
			} // END if

			TIM1->CCR1 = dutyCycleRegValue;	// Load the duty cycle register
		}// END if

		// If the required state is LO_STATE, then the duty cycle should
		//	be applied with reference to the high-side switches (75% dc means
		//	75%dc on low-side)
		else if(state == MPWM_LO_STATE)
		{
			// A check to ensure that the phase is not already in a low state
			//	will keep the software from re-clearing and re-loading registers
			//	that won't actually change
			if(MPWM_motorPhase.stateA != MPWM_LO_STATE){
				TIM1->CCER |= (uint16_t)(0b0101 << 0);	// TIM1 CH1 and CH1N on, active high

				TIM1->CCMR1 &= 0xff00;	// clear CC1 bits to default
				TIM1->CCMR1 |= (uint16_t)(0b01110000 << 0);	// pwm mode 2

				MPWM_motorPhase.stateA = MPWM_LO_STATE;
			}

			TIM1->CCR1 = dutyCycleRegValue;	// Load the duty cycle register
		}// END else-if

		// If the required state is DORMANT, then turn both high phase
		//	and low phase off
		else
		{
			TIM1->CCER &= 0xfff0;	// turn off pwm output, high-side and low-side
			MPWM_motorPhase.stateA = MPWM_DORMANT;
		} //END else
	}

	// Loads PH_C variables and registers.
	//	Same idea as PH_A logic above, just without comments
	else if(phase == MPWM_PH_B)
	{
		if(state == MPWM_HI_STATE)
		{
			if(MPWM_motorPhase.stateB != MPWM_HI_STATE){
				TIM1->CCER |= (uint16_t)(0b0101 << 4);

				TIM1->CCMR1 &= 0x00ff;
				TIM1->CCMR1 |= (uint16_t)(0b01100000 << 8);
				MPWM_motorPhase.stateB = MPWM_HI_STATE;
			}

			TIM1->CCR2 = dutyCycleRegValue;
		}
		else if(state == MPWM_LO_STATE)
		{
			if(MPWM_motorPhase.stateB != MPWM_LO_STATE)
			{
				TIM1->CCER |= (uint16_t)(0b0101 << 4);

				TIM1->CCMR1 &= 0x00ff;
				TIM1->CCMR1 |= (uint16_t)(0b01110000 << 8);

				MPWM_motorPhase.stateB = MPWM_LO_STATE;
			}

			TIM1->CCR2 = dutyCycleRegValue;
		}
		else
		{
			TIM1->CCER &= 0xff0f;
			MPWM_motorPhase.stateB = MPWM_DORMANT;
		}

	// Loads PH_C variables and registers.
	//	Same idea as PH_A logic above, just without comments
	}else if(phase == MPWM_PH_C)
	{
		if(state == MPWM_HI_STATE)
		{
			if(MPWM_motorPhase.stateC != MPWM_HI_STATE)
			{
				TIM1->CCER |= (uint16_t)(0b0101 << 8);

				TIM1->CCMR2 &= 0xff00;
				TIM1->CCMR2 |= (uint16_t)(0b01100000 << 0);

				MPWM_motorPhase.stateC = MPWM_HI_STATE;
			}

			TIM1->CCR3 = dutyCycleRegValue;
		}
		else if(state == MPWM_LO_STATE)
		{
			if(MPWM_motorPhase.stateC != MPWM_LO_STATE)
			{
				TIM1->CCER |= (uint16_t)(0b0101 << 8);

				TIM1->CCMR2 &= 0xff00;
				TIM1->CCMR2 |= (uint16_t)(0b01110000 << 0);

				MPWM_motorPhase.stateC = MPWM_LO_STATE;
			}

			TIM1->CCR3 = dutyCycleRegValue;
		}
		else
		{
			TIM1->CCER &= 0xf0ff;
			MPWM_motorPhase.stateC = MPWM_DORMANT;
		}
	}

	return;
} // END MPWM_setPhaseDutyCycle()

/***************************************************************************
 * 	Function:	void TIM1_IRQHandler(void);
 *
 * 	Purpose:	To count milliseconds for use in other modules
 *
 * 	Parameters:	none
 *
 * 	Notes:		none
 *
 * 	Example:	none
 ***************************************************************************/
void
TIM1_CC_IRQHandler(void)
{
	GPIO_setOutputPin(GPIO_PORT_A, 5);
	ADC_startAdcConversion();

	TIM1->SR = 0;
	GPIO_clearOutputPin(GPIO_PORT_A, 5);
	return;
} // END TIM2_IRQHandler



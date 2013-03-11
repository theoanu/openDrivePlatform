

/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#include <stdbool.h>
#include "stm32f10x_adc.h"
#include "adc.h"

#define NULL 0

typedef struct
{
	uint32_t *adc1InterruptPtr;
	uint32_t *adc2InterruptPtr;
} _adc;

_adc adc;

void ADC_initAdc1(void);
void ADC_initAdc2(void);
void (*adc1InterruptPtr)(void) = NULL;
void (*adc2InterruptPtr)(void) = NULL;

/***************************************************************************
 * Function:	void initAdc(void)
 *
 * Purpose:		This function is called to initialize the ADC for operation
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	none
 ***************************************************************************/
void
ADC_initAdc(void)
{
	ADC_initAdc1();
	ADC_initAdc2();

	// Enables interrupt in NVIC
	NVIC_EnableIRQ(ADC1_2_IRQn);

	return;
}

/***************************************************************************
 * Function:	void initAdc1(void)
 *
 * Purpose:		This function is called to initialize ADC1 for operation
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	Most ADC1 registers
 ***************************************************************************/
void
ADC_initAdc1(void)
{
	ADC1->CR1 |= (uint32_t)(1 << 8);			// enable scan mode


	ADC1->CR2 |= (uint32_t)((1 << 1)			// ADC1 continuous conversion mode
							+ (1 << 15)			// ADC1 conversion on external event enabled
							+ (0b111 << 12));	// ADC1 conversion triggered on setting of SWSTART

	ADC1->JSQR |= (uint32_t)((0b10 << 20)			// ADC1 3 conversions to complete
							+ (0 << 5)				// in0 first
							+ (1 << 10)				// in1 second
							+ (2 << 15));			// in2 third

	ADC1->CR2 |= (uint32_t)(1);			// ADC1 on

	ADC1->CR2 |= (uint32_t)(1 << 2);	// start calibration of the ADC
	while((ADC1->CR2 & (1 << 2)) > 0);	// wait for calibration to complete

	return;
} // END ADC_initAdc1()

/***************************************************************************
 * Function:	void initAdc2(void)
 *
 * Purpose:		This function is called to initialize ADC2 for operation
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	Most ADC2 registers
 ***************************************************************************/
void
ADC_initAdc2(void)
{
	ADC2->CR1 |= (uint32_t)(1 << 8);			// enable scan mode

	ADC2->CR2 |= (uint32_t)((1 << 1)			// ADC2 continuous conversion mode
							+ (1 << 15)			// ADC2 conversion on external event enabled
							+ (0b111 << 12));	// ADC2 conversion triggered on setting of SWSTART

	ADC2->JSQR |= (uint32_t)((0b10 << 20)			// ADC2 2 conversions to complete
							+ (4 << 5)				// in4 first
							+ (3 << 10)				// in3 first
							+ (7 << 15));			// in7 second


	ADC2->CR2 |= (uint32_t)(1);			// ADC2 on

	ADC2->CR2 |= (uint32_t)(1 << 2);	// start calibration of the ADC
	while((ADC2->CR2 & (1 << 2)) > 0);	// wait for calibration to complete

	return;
} // END ADC_initAdc2()

/***************************************************************************
 * Function:	void ADC_startAdcConversions(void)
 *
 * Purpose:		This function is called to start ADC conversion process
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	ADC1->CR2
 ***************************************************************************/
void
ADC_startAdcConversion(void)
{
	ADC1->CR2 |= (uint32_t)(1 << 21);	// start conversions JSWSTART
	ADC2->CR2 |= (uint32_t)(1 << 21);	// start conversions JSWSTART
}

/***************************************************************************
 * Function:	uint16_t ADC_getVoltage(_adcSample voltageSource)
 *
 * Purpose:		This function is called in order to get a particular value
 * 					from the ADC module
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	none
 ***************************************************************************/
uint16_t
ADC_getVoltage(_adcSample voltageSource)
{
	uint16_t voltage = 0;

	switch(voltageSource)
	{
		case ADC_PH_A:
			voltage = ADC1->JDR1;
			break;

		case ADC_PH_B:
			voltage = ADC1->JDR2;
			break;

		case ADC_PH_C:
			voltage = ADC1->JDR3;
			break;

		case ADC_CONTROL_VOLTAGE:
			voltage = ADC2->JDR1;
			break;

		case ADC_V_BUS:
			voltage = ADC2->JDR2;
			break;

		case ADC_I_BUS:
			voltage = ADC2->JDR3;
			break;

		default:
			break;

	}

	return voltage;
}

/***************************************************************************
 * Function:	void ADC_initAdc1Interrupt(void (*addressPtr)(void))
 *
 * Purpose:		This function is called in order to enable interrupts on ADC1
 * 					and also to pass the address of higher-level code in order
 * 					to assign the interrupt to higher-level code.
 *
 * Parameters:	void (*addressPtr)(void) - the address of code to be executed
 * 											when an ADC1 interrupt occurs
 *
 * Returns:		none
 *
 * Globals affected:	*adc1InterruptPtr
 ***************************************************************************/
void
ADC_initAdc1Interrupt(void (*addressPtr)(void))
{
	adc1InterruptPtr = addressPtr;

	ADC1->CR1 |= (uint32_t)(1 << 7);	// turn on interrupts for injected channels
	return;
}

/***************************************************************************
 * Function:	void ADC_initAdc2Interrupt(void (*addressPtr)(void))
 *
 * Purpose:		This function is called in order to enable interrupts on ADC2
 * 					and also to pass the address of higher-level code in order
 * 					to assign the interrupt to higher-level code.
 *
 * Parameters:	void (*addressPtr)(void) - the address of code to be executed
 * 											when an ADC2 interrupt occurs
 *
 * Returns:		none
 *
 * Globals affected:	*adc2InterruptPtr
 ***************************************************************************/
void
ADC_initAdc2Interrupt(void (*addressPtr)(void))
{
	adc2InterruptPtr = addressPtr;

	ADC2->CR1 |= (uint32_t)(1 << 7);	// turn on interrupts for injected channels
	return;
}

/***************************************************************************
 * Function:	void ADC_deinitAdc1Interrupt(void)
 *
 * Purpose:		This function is called in order to disable interrupts on ADC1
 * 					and to reset the pointer to NULL.
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	*adc1InterruptPtr
 ***************************************************************************/
void
ADC_deinitAdc1Interrupt(void)
{
	ADC1->CR1 &= (uint32_t)~(1 << 7);	// turn on interrupts for injected channels
	adc1InterruptPtr = NULL;

	return;
}

/***************************************************************************
 * Function:	void ADC_deinitAdc2Interrupt(void)
 *
 * Purpose:		This function is called in order to disable interrupts on ADC2
 * 					and to reset the pointer to NULL.
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	*adc2InterruptPtr
 ***************************************************************************/
void
ADC_deinitAdc2Interrupt(void)
{
	ADC2->CR1 &= (uint32_t)~(1 << 7);	// turn on interrupts for injected channels
	adc2InterruptPtr = NULL;

	return;
}

/***************************************************************************
 * Function:	void ADC1_2_IRQHandler(void)
 *
 * Purpose:		ADC interrupt that occurs when either ADC1 or ADC2 are executed.
 * 					This function is currently configured to distinguish between
 * 					ADC1 and ADC2 interrupts and to call higher-level code that
 * 					was previously assigned when the interrupt was enabled.
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	none
 ***************************************************************************/

void
ADC1_2_IRQHandler(void)
{
	bool adc1IntEnabled = (ADC1->CR1 & (uint32_t)(0b1 << 7)) >> 7;
	bool adc1IntTriggered = (ADC1->SR & (uint32_t)(0b1 << 2)) >> 2;

	// Action if ADC1 interrupt is enabled and ADC1 triggered the interrupt
	if(adc1IntTriggered && adc1IntEnabled)
	{
		ADC1->SR &= (uint32_t)~(0b1 << 2);		// clear the interrupt flag

		(*adc1InterruptPtr)();				// call the function that was assigned to this pointer
	}

	bool adc2IntEnabled = (ADC2->CR1 & (uint32_t)(0b1 << 7)) >> 7;
	bool adc2IntTriggered = (ADC2->SR & (uint32_t)(0b1 << 2)) >> 2;

	// Action if ADC2 interrupt is enabled and ADC2 triggered the interrupt
	if(adc2IntEnabled && adc2IntTriggered)
	{
		ADC2->SR &= (uint32_t)~(0b1 << 2);		// clear the interrupt flag

		(*adc2InterruptPtr)();				// call the function that was assigned to this pointer
	}

	return;
}




/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#include "stm32f10x_tim.h"
#include "misc.h"
#include "rcPwm.h"
#include "milliSecTimer.h"
#include "gpio.h"

typedef struct rcpwm
{
	uint16_t longestPulseTime;			// Corresponds to 100% speed demand
	uint16_t shortestPulseTime;			// Corresponds to 0% speed demand
	uint32_t lastPulseReceivedTimeAbs;	// Indicates the time stamp of the last pulse

	uint16_t demand;					// This is the Q16 speed demand
} _rcpwm;

_rcpwm rcPwm;

/***************************************************************
 * Function:	void RCPWM_initRcPwm(void)
 *
 * Purpose:		To initialize the RC PWM input so that it will
 * 					measure positive pulse widths, compare them
 * 					to past pulse widths received, and outputs
 * 					a fixed-point representation of a percent
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	TIM3->...
 * 						rcPwm.longestPulseTime = 31500,
 * 						rcPwm.shortestPulseTime = 22500
 **************************************************************/
void
RCPWM_initRcPwm(void)
{
	// Setup RC PWM input pin
	// RC PWM input pin is not remapped
	GPIO_pinSetup(GPIO_PORT_A, 6, GPIO_FLOATING_INPUT);

	/* TIM3 clock enable @36MHz */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	// Enables interrupt in NVIC
	NVIC_EnableIRQ(TIM3_IRQn);

	// Interrupt enable on Capture/Compare 2 of TIM3, Chan 1
	TIM3->DIER |= (uint16_t)(0b1 << 2);
	//TIM3->DIER |= (uint16_t)((0b1 << 2) + (0b1 << 1)); // uncomment this to enable interrupts on CC1 and CC2 (it works on CC1)

	// Capture current value of counter
	//	on and set appropriate interrupt
	//	flags on capture (CC1 and CC2)
	TIM3->EGR |= (uint16_t)((0b1 << 1)	// TI1
						+ (0b1 << 2));	// TI2

	// CC1 is an input, IC1 is mapped on TI1
	// CC2 is an input, IC2 is mapped on TI2
	TIM3->CCMR1 |= (uint16_t)((0b10 << 8)	// TI2
						+ (0b01 << 0));		// TI1

	// CC1 capture occurs on rising edge, CC1 is enabled
	// CC2 capture occurs on falling edge, CC2 is enabled
	TIM3->CCER |= (uint16_t)((0b1 << 5)		// CC2 captured on falling
						+ (0b1 << 4)		// CC2 capture enabled
						+ (0b0 << 1)		// CC1 captured on rising
						+ (0b1 << 0));		// CC1 capture enabled

	// Reset the flag
	TIM3->SR = 0;

	// Prescaler loaded so that input clock is
	//	divided by two, thus, the clock is at
	//	18MHz, meaning that any pulse can be
	//	measured up to a maximum pulse width
	//	of 3.64ms with a resolution of +/-27ns
	TIM3->PSC = 2;

	// Initialize interrupts on TIM3
	NVIC_InitTypeDef nvicInitStruct;
	nvicInitStruct.NVIC_IRQChannel = TIM3_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStruct);

	// Enable the counter
	TIM3->CR1 |= 0x0001;

	// Initialize rcPwm.longestPulseTime and rcPwm.shortestPulseTime
	//	to reasonable starting values (maybe 1.25ms and 1.75ms?)
	rcPwm.longestPulseTime = 47939;
	rcPwm.shortestPulseTime = 23975;

	return;
} // END initRcPwm()

/***************************************************************
 * Function:	uint32_t RCPWM_getSpeedDemand(void)
 *
 * Purpose:		To get the speed demand in a fixed-point fractional
 * 					format
 *
 * Parameters:	none
 *
 * Returns:		rcPwm.demand
 *
 * Globals affected:	none
 **************************************************************/
uint16_t
RCPWM_getSpeedDemand(void)
{
	// TODO: Check to ensure that pulses are being received
	//	and, if not, return 0
	if(MSTMR_getMilliSeconds() > (rcPwm.lastPulseReceivedTimeAbs + 20))
	{
		rcPwm.demand = 0;
	}

	return rcPwm.demand;
} // END RCPWM_getSpeedDemand()

/***************************************************************
 * Function:	void TIM3_IRQHandler(void)
 *
 * Purpose:		To get the absolute time for the rising edge and
 * 					for the falling edge and - using the saved
 * 					history - calculates a fixed-point representation
 * 					of the percentage.
 *
 * Parameters:	none
 *
 * Returns:		none
 *
 * Globals affected:	rcPwm.demand
 **************************************************************/
void
TIM3_IRQHandler(void)
{
	// Find the current pulse time.
	//	pulseWidth = risingEdgeTime - fallingEdgeTime
	uint16_t pulseWidth = TIM3->CCR2 - TIM3->CCR1;

	// Separate the min pulse width from the signal pulse width
	uint32_t signalPulseWidth;
	if(pulseWidth > rcPwm.shortestPulseTime)
		signalPulseWidth = pulseWidth - rcPwm.shortestPulseTime;
	else
		signalPulseWidth = 0;

	// Determine the range of the pulse widths
	uint32_t range = rcPwm.longestPulseTime - rcPwm.shortestPulseTime;

	// Calculate the signal pulse width as a
	//	percentage of the total range of the pulse
	uint32_t result = (signalPulseWidth << 16)/range;
	if(result > 65535)
		result = 65535;

	// Save the result as a 16-bit number
	rcPwm.demand = (uint16_t)result;

	// Save the time that this pulse was received
	rcPwm.lastPulseReceivedTimeAbs = MSTMR_getMilliSeconds();

	// Reset the flag
	TIM3->SR = 0;

	return;
} // end TIM3_IRQHandler()


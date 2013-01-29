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

/* User-generated libs */
#include "osc.h"
#include "milliSecTimer.h"

typedef struct{
	uint32_t milliSeconds;
} _timer;

_timer MSTMR_timer;


/***************************************************************************
 * 	Function:	void MSTMR_initMilliSecTimer(void);
 *
 * 	Purpose:	To initialize the millisecond timer for use in other modules
 *
 * 	Parameters:	none
 *
 * 	Notes:		none
 *
 * 	Example:	none
 ***************************************************************************/
void
MSTMR_initMilliSecTimer(void)
{
	uint32_t timerTwoFreq;
	uint16_t arrValue;

	/* TIM2 clock enable @36MHz */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	// Enables interrupt in NVIC
	NVIC_EnableIRQ(TIM2_IRQn);

	// Interrupt enable on Capture/Compare 4
	TIM2->DIER |= (uint16_t)(0b1 << 4);

	// Reset the flag
	TIM2->SR = 0;

	// Prescaler loaded so that input clock is divided by two.
	TIM2->PSC = 1;

	// Calculate the timer 2 input clock frequency based on the prescalers
	timerTwoFreq = OSC_getClockFreq() >> 1;

	// Calculate the ARR value for timer 2 in order to create an overflow at 1ms
	arrValue = (uint16_t)(timerTwoFreq/1000);
	TIM2->ARR = arrValue;

	// Enable the counter
	TIM2->CR1 |= 0x0001;

	// Reset the milliSeconds timer
	MSTMR_timer.milliSeconds = 0;

	return;
} // END MSTMR_initMilliSecTimer()


/***************************************************************************
 * 	Function:	void TIM2_IRQHandler(void);
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
TIM2_IRQHandler(void)
{
	MSTMR_timer.milliSeconds++;
	TIM2->SR = 0;
	return;
} // END TIM2_IRQHandler

/***************************************************************************
 * 	Function:	void TIM2_IRQHandler(void);
 *
 * 	Purpose:	To retrieve the current milliseconds value for use in other modules
 *
 * 	Parameters:	none
 *
 * 	Notes:		none
 *
 * 	Example:	none
 ***************************************************************************/
uint32_t
MSTMR_getMilliSeconds(void)
{
	return MSTMR_timer.milliSeconds;
} // END MSTMR_getMilliSeconds()

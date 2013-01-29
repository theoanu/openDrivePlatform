/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/

#include "osc.h"

typedef struct{
	uint32_t clockFreq;
} __osc;

__osc oscillator;

/*
 * Private function declarations
 */
void OSC_initHseClock(void);
void OSC_initHsiClock(void);

// Function implementations here
void
OSC_initClock(void)
{
	#ifdef HSE_CLK
		OSC_initHseClock();
	#else
		OSC_initHsiClock();
	#endif

	return;
} // END OSC_initClock()

void
OSC_initHseClock(void)
{
	/* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	/* Flash 2 wait state */
	FLASH_SetLatency(FLASH_Latency_2);

	// HSE On
	RCC->CR |= (uint32_t)(0b1 << 16);

	// Wait for HSE to start
	bool hseRdy = false;

	while(!hseRdy){
		hseRdy = (bool)(0b1 & (RCC->CR  >> 17));
	}

	// PLLSRC = HSE
	RCC->CFGR |= (uint32_t)(0b1 << 16);

	// pllOutput = pllInput x 9 = 8MHz x 9 = 72MHz
	RCC->CFGR |= (uint32_t)(0b0111 << 18);

	// PLL ON
	RCC->CR |= (uint32_t)(0b1 << 24);

	// Wait for PLL to lock
	bool pllRdy = false;

	while(pllRdy == false){
		pllRdy = (bool)(0b1 & (RCC->CR >> 25));
	}

	// Set system clock as PLL
	RCC->CFGR |= (uint32_t)(0b10 << 0);

	// Wait for system clock to complete switch
	uint32_t clock = 0;

	while(clock != 0b10){
		clock = (uint32_t)((RCC->CFGR >> 2) & 0x00000003);
	}

	// APB1 = 36MHz, APB2 = 72MHz
	RCC->CFGR |= (uint32_t)(0b100 << 8);

	// ADCPRE = PCLK2/6 = 72MHz/6 = 12MHz (14MHz max)
	RCC->CFGR |= (uint32_t)(0b10 << 14);

	// Enable peripheral clocks
	RCC->APB2ENR |= (uint32_t)((1 << 11)		// TIM1
					+ (1 << 10)					// ADC2
					+ (1 << 9)					// ADC1
					+ (1 << 4)					// IO port C
					+ (1 << 3)					// IO port B
					+ (1 << 2));				// IO port A

	RCC->APB1ENR |= (uint32_t)((1 << 17)		// USART2
					+ (1 << 1));				// TIM3

	oscillator.clockFreq = 72000000;

	return;
} // END OSC_initHseClock()

void
OSC_initHsiClock(void)
{
	/* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	/* Flash 2 wait state */
	FLASH_SetLatency(FLASH_Latency_2);

	// pllOutput = pllInput x 16 = 8MHz/2 x 16 = 64MHz
	RCC->CFGR |= (uint32_t)(0b1111 << 18);

	// PLL ON
	RCC->CR |= (uint32_t)(0b1 << 24);

	// Wait for PLL to lock
	bool pllRdy = false;

	while(pllRdy == false){
		pllRdy = (bool)(0b1 & (RCC->CR >> 25));
	}

	// Set system clock as PLL
	RCC->CFGR |= (uint32_t)(0b10 << 0);

	// Wait for system clock to complete switch
	uint32_t clock = 0;

	while(clock != 0b10){
		clock = (uint32_t)((RCC->CFGR >> 2) & 0x00000003);
	}

	// APB1 = 32MHz, APB2 = 64MHz
	RCC->CFGR |= (uint32_t)(0b100 << 8);

	// ADCPRE = PCLK2/6 = 64MHz/6 = 10.67MHz (14MHz max)
	RCC->CFGR |= (uint32_t)(0b10 << 14);

	// Enable peripheral clocks
	RCC->APB2ENR |= (uint32_t)((1 << 11)		// TIM1
					+ (1 << 10)		// ADC2
					+ (1 << 9)		// ADC1
					+ (1 << 4)		// IO port C
					+ (1 << 3)		// IO port B
					+ (1 << 2));	// IO port A

	RCC->APB1ENR |= (uint32_t)((1 << 17)		// USART2
					+ (1 << 1));	// TIM3

	oscillator.clockFreq = 64000000;

	return;
} // END InitHsiClock()

uint32_t
OSC_getClockFreq(void)
{
	return oscillator.clockFreq;
}



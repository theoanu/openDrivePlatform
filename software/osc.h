/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/

#ifndef OPENESC_V01
#define OPENESC_V01

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

#include <stdbool.h>

// Relevant defines that are hardware-related
#define HSE_CLK

/*
 * Public function declarations
 */
void OSC_initClock(void);
uint32_t OSC_getClockFreq(void);


#endif

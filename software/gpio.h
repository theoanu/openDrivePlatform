/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#include <stdbool.h>
#include <stdint.h>

#ifndef GPIO_H
#define GPIO_H

#define GPIO_INPUT_ANALOG			0b0000
#define GPIO_FLOATING_INPUT			0b0100
#define GPIO_INPUT_PU_OR_PD			0b1000
#define GPIO_OUTPUT_PP				0b0011
#define GPIO_OUTPUT_OPEN_DRN		0b0111
#define GPIO_OUTPUT_ALT_PP			0b1011
#define GPIO_OUTPUT_ALT_OPEN_DR		0b1111

typedef enum
{
	GPIO_PORT_A,
	GPIO_PORT_B
} _port;

#define GPIO_LO	0
#define GPIO_HI	1

void GPIO_pinSetup(_port port, uint16_t pin, uint8_t pinState);
void GPIO_setOutputPin(_port port, uint16_t pin);
void GPIO_clearOutputPin(_port port, uint16_t pin);
bool GPIO_readInput(_port port, uint16_t pin);

#endif

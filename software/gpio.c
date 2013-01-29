/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#include "stm32f10x_gpio.h"

#include "gpio.h"

void
GPIO_pinSetup(_port port, uint16_t pin, uint8_t pinState)
{
	switch(port)
	{
		case GPIO_PORT_A:
		{
			if((pin >= 0) && (pin <= 7)){
				// clear the appropriate bits, then set the appropriate bits
				GPIOA->CRL &= (uint32_t)~(0b1111 << (pin * 4));
				GPIOA->CRL |= (uint32_t)(pinState << (pin * 4));
			}else if((pin >= 8) && (pin <= 15)){
				// clear the appropriate bits, then set the appropriate bits
				GPIOA->CRH &= (uint32_t)~(0b1111 << ((pin - 8) * 4));
				GPIOA->CRH |= (uint32_t)(pinState << ((pin - 8) * 4));
			}else{
				/*
				 * This is a programmer's trap.  If the code gets to this while loop,
				 * then an invalid pin within this port has been specified.
				 */
				while(1);
			}
			break;
		} // END case GPIO_PORT_A

		case GPIO_PORT_B:
		{
			if((pin >= 0) && (pin <= 7)){
				// clear the appropriate bits, then set the appropriate bits
				GPIOB->CRL &= (uint32_t)~(0b1111 << (pin * 4));
				GPIOB->CRL |= (uint32_t)(pinState << (pin * 4));
			}else if((pin >= 8) && (pin <= 15)){
				// clear the appropriate bits, then set the appropriate bits
				GPIOB->CRH &= (uint32_t)~(0b1111 << ((pin - 8) * 4));
				GPIOB->CRH |= (uint32_t)(pinState << ((pin - 8) * 4));
			}else{
				/*
				 * This is a programmer's trap.  If the code gets to this while loop,
				 * then an invalid pin within this port has been specified.
				 */
				while(1);
			}

			break;
		} // END case GPIO_PORT_B

		default:
		{
			/*
			 * This is a programmer's trap.  If the code gets to this while loop,
			 * then an invalid port has been specified.
			 */
			while(1);
			break;
		}
	} // END switch

	return;
} // END GPIO_pinSetup()

void
GPIO_setOutputPin(_port port, uint16_t pin)
{
	switch(port)
	{
		case GPIO_PORT_A:
		{
			//GPIOA->BSSR |= (uint32_t)(0b1 << pin);
			GPIOA->ODR |= (uint32_t)(0b1 << pin);

			break;
		}

		case GPIO_PORT_B:
		{
			//GPIOB->BSSR |= (uint32_t)(0b1 << pin);
			GPIOB->ODR |= (uint32_t)(0b1 << pin);

			break;
		}

		default:
		{
			/*
			 * This is a programmer's trap.  If the code gets to this while loop,
			 * then an invalid port has been specified.
			 */
			while(1);
			break;
		}
	} // END switch

	return;
} // END GPIO_setOutputPin()

void
GPIO_clearOutputPin(_port port, uint16_t pin)
{
	switch(port)
	{
		case GPIO_PORT_A:
		{
			//GPIOA->BSSR |= (uint32_t)(0b1 << (pin + 16));
			GPIOA->ODR &= (uint32_t)~(0b1 << pin);

			break;
		}

		case GPIO_PORT_B:
		{
			//GPIOB->BSSR |= (uint32_t)(0b1 << (pin + 16));
			GPIOB->ODR &= (uint32_t)~(0b1 << pin);

			break;
		}

		default:
		{
			/*
			 * This is a programmer's trap.  If the code gets to this while loop,
			 * then an invalid port has been specified.
			 */
			while(1);
			break;
		}
	} // END switch

	return;
} // END GPIO_clearOutputPin()

bool
GPIO_readInput(_port port, uint16_t pin)
{
	bool pinState = 0;

	switch(port)
	{
		case GPIO_PORT_A:
		{
			pinState = (bool)((GPIOA->IDR & (uint32_t)(0b1 << pin)) >> pin);
			break;
		}

		case GPIO_PORT_B:
		{
			pinState = (bool)((GPIOB->IDR & (uint32_t)(0b1 << pin)) >> pin);

			break;
		}

		default:
		{
			/*
			 * This is a programmer's trap.  If the code gets to this while loop,
			 * then an invalid port has been specified.
			 */
			while(1);
			break;
		}
	} // END switch

	return pinState;
} // END GPIO_readInput()

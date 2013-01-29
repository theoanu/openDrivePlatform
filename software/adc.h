/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/

typedef enum
{
	ADC_PH_A,
	ADC_PH_B,
	ADC_PH_C,
	ADC_CONTROL_VOLTAGE,
	ADC_V_BUS,
	ADC_I_BUS
} _adcSample;

void ADC_initAdc(void);
void ADC_startAdcConversion(void);
uint16_t ADC_getVoltage(_adcSample voltageSource);
void ADC_initAdc1Interrupt(void (*addressPtr)(void));
void ADC_initAdc2Interrupt(void (*addressPtr)(void));
void ADC_deinitAdc1Interrupt(void);
void ADC_deinitAdc2Interrupt(void);


/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/
#ifndef MOTPWM_H
#define MOTPWM_H

typedef enum
{
	MPWM_PH_A,
	MPWM_PH_B,
	MPWM_PH_C,
} _phaseName;

typedef enum
{
	MPWM_DORMANT,
	MPWM_HI_STATE,
	MPWM_LO_STATE
} _phaseState;

void MPWM_initMotorPwm(void);
void MPWM_setMotorPwmFreq(uint16_t pwmFrequency);
void MPWM_setPhaseDutyCycle(uint8_t phase, uint8_t state, uint16_t dutyCycle);
void MPWM_setAdcSamplingTime(uint16_t samplingTime);

#endif

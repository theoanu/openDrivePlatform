/**************************************************
 * This file is Public Domain
 *
 * This file is distributed in the hope that it will
 *	be useful, but WITHOUT ANY WARRANTY; without
 *	even the implied warranty of MERCHANTABILITY
 *	or FITNESS FOR A PARTICULAR PURPOSE.
*************************************************/

#ifndef RCPWM_H
#define RCPWM_H

#define MIN_RC_PULSE_WIDTH	18000

void RCPWM_initRcPwm(void);
uint16_t RCPWM_getSpeedDemand(void);

#endif

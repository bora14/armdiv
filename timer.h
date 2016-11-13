/*
 * timer.h
 *
 *  Created on: 29.11.2015
 *      Author: Ivan
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "board.h"
#include "MDR32F9Qx_timer.h"

int Timer1_Configure(Preset_t * preset);

int Timer3_Configure(Preset_t * preset);

#endif /* TIMER_H_ */

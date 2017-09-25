/** \file
 * dpll.h
 *
 *  Created on: 29.11.2015
 *      Author: Ivan
 */

#ifndef DPLL_H_
#define DPLL_H_

#include "board.h"

/**
 *  Режимы работы схемы слежения
 */
enum dpll_mode
{
	DPLL_MODE_ROUGH = 0,	///< Грубый поиск
	DPLL_MODE_FINE = 1,		///< Точный поиск
	DPLL_MODE_DRAW = 2,		///< Втягивание
	DPLL_MODE_TRACK = 3,	///< Слежение
	DPLL_MODE_FAIL = 4		///< Срыв сопровождения
};

int32_t dpll_Init(Preset_t * preset_);

uint8_t dpll_UpdFlg();

void dpll_SetUpdFlg();

void dpll_ClearUpdFlg();

void dpll_Update();

void dpll_Reset();

#endif /* DPLL_H_ */

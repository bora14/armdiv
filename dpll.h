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
 * Количество возможных значение коэффициента усиления петлевого фильтра
 */
#define DPLL_GAIN_ITEMS	7

/**
 * Инициализация системы ФАПЧ
 */
int32_t dpll_Init(Preset_t * preset_);

/**
 * Указатель на структуру в которой храняться параметры ФАПЧ
 */
dpll_t * dpll_Get();

/**
 * \brief Вычисление выходного значения петлевого фильтра.
 *
 * Функция размещается в оперативной памяти для ускорения работы
 * всей системы.
 */
int32_t dpll_Filt(dpll_t * arg);
/**
 * Сброс петлевого фильтра
 */
void dpll_ClearFilt();
/**
 * Флаг готовности данных фазового детектора
 */
uint8_t dpll_UpdFlg();
/**
 * Обнуление значений фазового детектора
 */
void dpll_ClearAcc();
/**
 * Обнуление выходного значения петлевого фильтра
 */
void dpll_ClearPhi();
/**
 * Установка флага готовности фазового детектора
 */
void dpll_SetUpdFlg();
/**
 * Сброс флага готовности фазового детектора
 */
void dpll_ClearUpdFlg();
/**
 * Обновление состояния ФАПЧ
 */
void dpll_Update();
/**
 * Петлевой фильтр.
 */
float dpll_LoopFilter(float x, float *z, float *a, uint8_t order);

#endif /* DPLL_H_ */

/**
 * \file termo.h
 * \brief
 *
 * Файл содержит объявления функций для
 * снятия показаний датчика температуры.
 */

#ifndef TERMO_H_
#define TERMO_H_

#include "board.h"

/**
 * Инициализация переменных для работы с датчиками температуры.
 *
 */
void termo_Init(Preset_t * preset);

/**
 * Оцифровка напряжения на выходе датчика температуры.
 */
uint16_t termo_Val();

/**
 * Выбор используемого датчика температуры.
 */
void termo_SetChan(uint8_t chan);

#endif /* TERMO_H_ */

﻿/*
 * \file pressure.h
 *
 * Тарировка модулей давления предназначена
 * для получения выходного значения давления, зависящего от
 * двух параметров (периода и температуры датчика).
 * Тарировка производится с на 9 точках температуры
 * рабочего диапазона (-60...+100 С°) с равномерным шагом 50 ГПа
 * в диапазоне  давлений 5...1350 ГПа (в начале диапазона 5, 8,
 * 50 далее с шагом 50 до 1350). Запись данных осуществляется,
 * последовательно после выдержки модуля на данной температуре
 * не менее 2 часов начиная с температуры -60.
 */

#ifndef PRESSURE_H_
#define PRESSURE_H_

#include "board.h"


#define PRESSURE_P_BEGIN			(5)
#define PRESSURE_P_END				(1350)
#define PRESSURE_P_STEP				(50)
#define PRESSURE_P_NUM				((PRESSURE_P_END - PRESSURE_P_BEGIN)/PRESSURE_P_STEP)
#define PRESSURE_TEMP_BEGIN			(-60)
#define PRESSURE_TEMP_END			(100)
#define PRESSURE_TEMP_STEP			(15)
#define PRESSURE_TEMP_NUM			((PRESSURE_TEMP_END - PRESSURE_TEMP_BEGIN)/PRESSURE_TEMP_STEP)

/**
 * \brief Функция инициализации тарировочной таблицы.
 *
 * Тарировочая таблица, хранящаяся в ОЗУ, инициализируется
 * данными из ПЗУ. Это сделано для ускорения работы программы, так как
 * скорость считывания данных из ПЗУ ограничена 40 нс (см. спецификацию на 1986ВЕ9х).
 */
void pressure_InitTable();

/*
 * \brief Функция пересчета значения периода в давление (мм.рт.ст).
 *
 * Давление рассчитывается с помощью применения метода
 * двумерной интерполяции локальными кубическими сплайнами
 * по таблице значений функции с равноотстающими узлами.
 * Интерполяционная таблица  и коэффициенты масштабирования
 * являются результатами обработки тарировочных таблиц датчиков
 * давления, индивидуальны для каждого из них и хранятся в ПЗУ.
 *
 * \param[in] period - период, такт МК.
 * \param[in] temp - код значения температуры, МЗР.
 */
float pressure_Calc(uint16_t period, uint16_t temp);

#endif /* PRESSURE_H_ */

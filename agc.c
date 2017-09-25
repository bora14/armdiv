/**
 * \file
 * agc.c
 *
 *  Файл содержит функции для реализации
 *  автоматической регулировки усиления, а
 *  так же для оценки амплитуды входного сигнала
 *  и вычисления усредненного значения напряжения
 *  на выходе внутренненго ил внешнего термодиодов.
 */


#include "agc.h"
#include "adc.h"
#include "termo.h"
#include <stdlib.h>


static int32_t agc(int32_t x, uint32_t * acc, uint32_t * rem);

static Preset_t * preset;

/**
 * Инициализация АРУ.
 */
void agc_Init(Preset_t * _preset)
{
	preset = _preset;
}

/**
 * Установка порого срабатывания АРУ
 */
void agc_SetTH(int16_t th)
{
#ifdef AGC_ON
	preset->agc_th = th;
#endif
}

int32_t agc_Att0()
{
#ifdef AGC_ON
	return (preset->att0 * preset->dpll->T)/preset->T0;
#else
	return 0;
#endif
}

/**
 *  \brief Детектор огибающей входного сигнала
 *
 *  Данная функция используется для оценки
 *  амплитуды сигнала на выходе датчика.
 */
uint32_t agc_Amp()
{
	static uint32_t rem;
	int32_t amp = 0;

	ADC_SetChannel(AGC_CHAN);

	ADC_Read((uint16_t * )&amp);

	amp = abs(ADC_MEAN - amp);

	agc(amp, &preset->amp, &rem);

	return preset->amp;
}

/**
 *  \brief Оценка напяжения на выходе термодатчика
 *
 *  Данная функция используется для оценки
 *  напряжения внутренненго или внешнего термодиодов.
 */
uint32_t agc_Termo()
{
	static uint32_t rem;
	int32_t termo = 0;

	termo_SetChan(preset->termo_src);

	ADC_Read((uint16_t * )&termo);

	agc(termo, &preset->termo, &rem);

	return preset->termo;
}

/**
 * \brief Moving Average
 */
int32_t agc(int32_t x, uint32_t * acc, uint32_t * rem)
{
	int32_t AMP;

	AMP = ((1 << AGC_RECU_D) - 1) * (*acc) + (*rem);

	(*acc) = (AMP >> AGC_RECU_D) + x;

	(*rem) = AMP & 0x1ff;

	return (*acc);
}

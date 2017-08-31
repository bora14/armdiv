/** \file
 *
 *
 */

#include "dpll.h"
#include "timer.h"
#include "ta.h"
#include "MDR32F9Qx_port.h"
#ifdef AGC_ON
#include "agc.h"
#include "adc.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <math.h>

static Preset_t * preset;

static dpll_t dpll;

static int32_t dAc_data[2] = {0};

static int8_t intr_data[2] = {0};


/**
 * Функция инициализации системы ФАПЧ
 */
int32_t dpll_Init(Preset_t * preset_)
{
	preset = preset_;

	preset->dpll = &dpll;

	dpll.T0 = preset->Tmax;
	dpll.T = 0.0f;
	dpll.Phi = 0;
	dpll.dAc = dAc_data;
	dpll.Acc = 0;
	dpll.cnt = 0;
	dpll.updflg = 0;
	dpll.intr = intr_data;
	dpll.dPhi = 0.0f;
	dpll.ld = 0;
	dpll.shift = 0;
	dpll.phase = 0;

	return 0;
}

/**
 * Функция dpll_Get возвращает указатель на структуру
 * в которой хранятся параметры ФАПЧ.
 */
dpll_t * dpll_Get()
{
	return preset->dpll;
}

/**
 * \defgroup dpll_Filt Петлевой фильтр
 * @{
 */

/**
 * Функция dpll_Filt вычисляет выходное значение
 * петлевого фильтра (т.е. Phi).
 */
int32_t dpll_Filt(dpll_t * dpll_)
{
	int32_t pos, phase;

	/* номер периода для которого получено
		текущее значение фазового детектора*/
	pos = dpll_->cnt ^ 0x1;


	if( (preset->dpll->interp_valid_ta & (TA_PHASE1_VALID_MSK | TA_PHASE2_VALID_MSK)) ==
			(TA_PHASE1_VALID_MSK | TA_PHASE2_VALID_MSK) )
	{
		phase = ta_PhaseInterp(&DPLL_TIMER->ARR, 1);
	}
	else
	{
		phase = preset->shift + preset->att/2;
	}

	if(preset->search > AMP_SEARCH_ACU)
		dpll.shift = (phase * (int32_t)DPLL_TIMER->ARR) / 360;
	else
	{
		dpll.shift = 0;
//		dpll_->dAc[pos] >>= 4;
	}

	dpll_->Acc = dpll_->dAc[pos] + dpll.shift;
//	if(abs(dpll.Acc) > (DPLL_TIMER->ARR >> 3))
//		dpll.Acc = (DPLL_TIMER->ARR >> 3) * sign(dpll.Acc);

//	dpll_->Acc = 200*sign(dpll_->dAc[pos] + dpll.shift);
	/* Вычисление выходного значения петлевого фильтра */
//	dpll.Phi = dpll_LoopFilter(dpll_->Acc/10.0f, NULL, NULL, 2);
	dpll.Phi = dpll_LoopFilter((3600*dpll_->Acc)/(int32_t)DPLL_TIMER->ARR, NULL, NULL, 2);

//	dpll.Phi += ((dpll.Acc - (float)dpll.shift) / 20.0f);
	dpll.Phi += (dpll.Acc / 20.0f);

	/**************************/

	return dpll_->Phi;
}
/** @} */

/**
 * \defgroup LoopFilter Петлевой фильтр
 * @{
 */
#define Bn	(0.01f)
//
#define w0 (Bn/0.53f)
//#define w0 (Bn/0.25f)
//
const static float T = 0.5f;
static float w[2] = {w0 * w0, 1.414f * w0};
//static float w[1] = {w0};
volatile static float A[2] = {0.0f};

/**
 * Функция, реализующая петлевой фильтр
 * порядка order, основанного на использовании
 * билинейного преобразования идеального интегратора.
 */
float dpll_LoopFilter(float x, float *z, float *a, uint8_t order)
{
	int8_t i;
	float y, r, acc;

	y = 0.0f;

	for(i = 0; i < order; i++)
	{
		r = y + w[i] * x;
		acc = A[i] + T * r;
		y = (acc + A[i])/2.0f;
		A[i] = acc;
	}

//	y += ((x - dpll.shift)/2.0f);
//	y += (x/8.0f - dpll.shift);
//	y += (x/4.0f);

	return y;
}
/** @} */

/**
 * Функция dpll_ClearFilt обнуляет значения,
 * хранящиеся в интеграторе.
 */
void dpll_ClearFilt()
{
	dpll.dPhi = 0.0f;
}

/**
 * Функция dpll_UpdFlg возврящает значение
 * флага готовности данных фазового детектора.
 */
uint8_t dpll_UpdFlg()
{
	return dpll.updflg;
}

/**
 * Функция dpll_SetUpdFlg устанавливает значение
 * флага готовности данных фазового детектора в 1.
 */
void dpll_SetUpdFlg()
{
	dpll.updflg = 1u;
}

/**
 * Функция dpll_ClearUpdFlg сбрасывает значение
 * флага готовности данных фазового детектора в 0.
 */
void dpll_ClearUpdFlg()
{
	dpll.updflg = 0u;
}

/**
 * Функция dpll_ClearAcc обнуляет значения
 * фазового детектора.
 */
void dpll_ClearAcc()
{
#ifndef FLL_ASSISTED
	dpll.dAc[0] = 0;
	dpll.dAc[1] = 0;
	A[0] = 0.0f;
	A[1] = 0.0f;
#else
	dpll.dAc.dAc = 0;
#endif
}

/**
 * Функция dpll_ClearPhi обнуляет выходное значение
 * петлевого фильтра.
 */
void dpll_ClearPhi()
{
	dpll.Phi = 0.0f;
}

/**
 * Функция dpll_Update содержит основной исполняемый код,
 * реализующий систему ФАПЧ.
 */
void dpll_Update()
{
	static int32_t dt = 1;

	if (preset->search == 0) // проверка наличия сигнала на входе
	{
		/* Свипирование */
		dpll.T0 += dt;
//		if(dpll.T0 > (DPLL_T_MAX + (DPLL_T_MIN - DPLL_T_MAX)/3))
//			dpll.T0 += 2*dt;
//		if(dpll.T0 > (DPLL_T_MAX + 2*(DPLL_T_MIN - DPLL_T_MAX)/3))
//			dpll.T0 += 4*dt;

		LED_Blink(LED1);

		if(dpll.T0 > preset->Tmin) // Проверка границ интеравала свипирования
		{
			dt = -1;
		}
		if(dpll.T0 < preset->Tmax)
		{
			dt = 1;
		}

		/*****************/

		dpll_ClearPhi(); // Обнуление выхода петлевого фильтра

		dpll_ClearAcc(); // Обнуление фазового детектора

		dpll_ClearFilt(); // Сброс значений петлевого фильтра

		dpll.intr[1] = 0;

		dpll.ld = 0;
	}
	else if(preset->search < (AMP_SEARCH_ACU >> 1))
	{
		preset->search++;
	}
	else
	{
		if(preset->search < AMP_SEARCH_ACU+2)
		{
			preset->search++;
		}
		LED_On(LED1);
#ifdef AGC_ON
		if(preset->termo_src == Amplitude)
		{
			agc();
		}
#endif
		dpll_Filt(preset->dpll); // Вычисление выходного значения петлевого фильтра

		dpll.ld = 1;
	}

	dpll.T = dpll.Phi + (float)dpll.T0; // Вычисление текущего периода выходного сигнала

	/* Проверка соответствия выходного сигнала диапазону рабочих частот */
	if (dpll.T < preset->Tmax)
	{
		dpll.T = preset->Tmax;
	}
	else if (dpll.T > preset->Tmin)
	{
		dpll.T = preset->Tmin;
	}
	else if ( isinf(dpll.T) || isnan(dpll.T))
	{
		dpll.T = preset->Tmin;
	}

	/* Обновление параметров таймера на котором реализована ФАПЧ */

//	MDR_TIMER1->CCR1 = (MDR_TIMER1->ARR >> 1) + (MDR_TIMER1->ARR & 0x1); // Обновление регистра захвата CCR1
//
//	MDR_TIMER1->CCR2 = MDR_TIMER1->CCR1 - (preset->att * MDR_TIMER1->ARR)/360; // Обновление регистра захвата CCR2
//	MDR_TIMER1->CCR21 = MDR_TIMER1->ARR - (preset->att * MDR_TIMER1->ARR)/360; // Обновление регистра захвата CCR21

	dpll_SetT(lroundf(dpll.T));
}

int dpll_SetT(uint32_t T)
{
	MDR_TIMER1->ARR = T;

	MDR_TIMER1->CCR1 = (MDR_TIMER1->ARR >> 1) + (MDR_TIMER1->ARR & 0x1); // Обновление регистра захвата CCR1

	MDR_TIMER1->CCR2 = MDR_TIMER1->CCR1 - (preset->att * MDR_TIMER1->ARR)/360; // Обновление регистра захвата CCR2
	MDR_TIMER1->CCR21 = MDR_TIMER1->ARR - (preset->att * MDR_TIMER1->ARR)/360; // Обновление регистра захвата CCR21

	return 1;
}

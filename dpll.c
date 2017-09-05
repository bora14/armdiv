/** \file Алгоритм слежения за частотой
 *
 * Схема слежения работает в четырех режимах:
 * DPLL_MODE_ROUGH - грубый поиск (быстрый)
 * DPLL_MODE_FINE - точный поиск (медленный)
 * DPLL_MODE_DRAW - втягивание
 * DPLL_MODE_TRACK - слежение
 *
 * Грубый поиск
 * При включении питания или после потери захвата
 * СЦВД начинает грубый (быстрый) поиск. Однако, после
 * положительного результата управление передается
 * на точный поиск.
 *
 * Точный поиск
 * При точном поиске скорость сканирования снижается,
 * что позволяет более точно определить максимум (алгоритм прежний).
 * После чего, схема слежения начинает работать в режиме втягивания.
 *
 * Режим втягивания
 * В этом режиме СЦВД не сканирует, а работает 256 тактов на частоте,
 * определенной при поиске. После чего, схема переходит в режим слежения.
 *
 * Режим слежения
 * В режиме слежения начинает работать фильтр петли ФАПЧ.
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
	dpll.search = 0;
	dpll.mode = DPLL_MODE_ROUGH;

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

	if(dpll.search > AMP_SEARCH_ACU)
		dpll.shift = (phase * (int32_t)DPLL_TIMER->ARR) / 360;
	else
	{
		dpll.shift = 0;
	}

	dpll_->Acc = dpll_->dAc[pos] + dpll.shift;
	/* Вычисление выходного значения петлевого фильтра */
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

	switch(dpll.mode)
	{
	case DPLL_MODE_ROUGH:

		if(dpll_search(preset) > 0)
		{
			dpll.mode = DPLL_MODE_FINE;
			dpll.search = 0;
			break;
		}

		/* Свипирование */
		dpll.T0 += dt;
		if(dpll.T0 > (DPLL_T_MAX + (DPLL_T_MIN - DPLL_T_MAX)/3))
			dpll.T0 += 2*dt;
		if(dpll.T0 > (DPLL_T_MAX + 2*(DPLL_T_MIN - DPLL_T_MAX)/3))
			dpll.T0 += 4*dt;

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

		break;

	case DPLL_MODE_FINE:

		if(dpll_search(preset) > 0)
		{
			dpll.mode = DPLL_MODE_DRAW;

			TIMER_ITConfig(DPLL_TIMER, TIMER_STATUS_CNT_ARR, DISABLE);

			TIMER_ITConfig(DPLL_TIMER, TIMER_STATUS_CCR_CAP_CH3, ENABLE);

			break;
		}

		/* Свипирование */
		dpll.T0 += dt;

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

		break;

	case DPLL_MODE_DRAW:

		if(dpll.search < (AMP_SEARCH_ACU >> 1))
		{
			dpll.search++;
		}
		else
		{
			dpll.mode = DPLL_MODE_TRACK;
		}

		break;

	case DPLL_MODE_TRACK:

		if(dpll.search < AMP_SEARCH_ACU+2)
		{
			dpll.search++;
		}

		dpll_Filt(preset->dpll); // Вычисление выходного значения петлевого фильтра

		dpll.ld = 1;

		break;
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

/**
 * \brief Поиск резонанса по амплитуде
 *
 * При запуске СЦВД начинается сканирование по частоте.
 * Одновременно с этим измеряется огибающая выходного
 * сигнала датчика. При сканировании в СЦВД запоминается preset->search_len выборок
 * частоты и амплитуды. После чего вычисляется разность амплитуд
 * между крайними и центральной точками выборки. Если разница
 * превышает порог, то значение амплитуды и частоты центральной точки
 * запоминаются. Далее, значение частоты сканирования повышается на единицу
 * и процесс повторяется. Если 20 раз подряд произошло событие детектирования
 * максимума, то из этих 20 значений выбирается частота, соответствующая
 * максимальной амплитуде и подставляется в схему слежения. После чего, через 256
 * тактов (что бы переходный процесс в датчике закончился) начинает
 * работать схема слежения.
 *
 */
int16_t dpll_search(Preset_t * preset)
{
	static uint16_t i, i1;
	static int32_t period[AMP_SEARCH_POINTS_NUM];
	static int32_t amp[AMP_SEARCH_POINTS_NUM];
	static int32_t local_max[20], local_idx[20];

	if((preset->dpll->search > 0) || (preset->es == 0))
		return preset->dpll->search;

	amp[i % preset->search_len] = preset->amp >> AGC_RECU_D;
	period[i % preset->search_len] = preset->dpll->T0;

	if(i++ < preset->search_len)
		return preset->dpll->search;

	int32_t DL, DE, P;
	P = amp[(i + (preset->search_len/2 + 1)) % preset->search_len];
	// Правая разность
	DL = P - amp[i % preset->search_len];
	// Левая разность
	DE = P - amp[(i + 1) % preset->search_len];

	if((DL > preset->search_th) && (DE > preset->search_th))
	{
		local_max[i1] = P;
		local_idx[i1] = (i + (preset->search_len/2 + 1)) % preset->search_len;
		i1++;
	}
	else
	{
		i1 = 0;
		return preset->dpll->search;
	}

	if(i1 > 19)
	{
		int idx = 0, i2, max_prev;
		max_prev = local_max[0];
		for(i2 = 1; i2 < 20; i2++)
		{
			if(local_max[i2] > max_prev)
			{
				max_prev = local_max[i2];
				idx = i2;
			}
		}
		preset->dpll->search = 1;

		preset->dpll->T0 = period[local_idx[idx]];

		dpll_SetT(period[local_idx[idx]]);

		i1 = 0; i = 0;

		return preset->dpll->search;
	}

	return preset->dpll->search;
}

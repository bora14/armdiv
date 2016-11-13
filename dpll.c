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
 * ������� ������������� ������� ����
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
 * ������� dpll_Get ���������� ��������� �� ���������
 * � ������� �������� ��������� ����.
 */
dpll_t * dpll_Get()
{
	return preset->dpll;
}

/**
 * \defgroup dpll_Filt �������� ������
 * @{
 */

/**
 * ������� dpll_Filt ��������� �������� ��������
 * ��������� ������� (�.�. Phi).
 */
int32_t dpll_Filt(dpll_t * dpll_)
{
	int32_t pos, phase;

	/* ����� ������� ��� �������� ��������
		������� �������� �������� ���������*/
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

	dpll.shift = (phase * (int32_t)DPLL_TIMER->ARR) / 360 ;

	dpll_->Acc = dpll_->dAc[pos] + dpll.shift;
	if(abs(dpll.Acc) > 1000)
		dpll.Acc = 1000 * sign(dpll.Acc);
//	if(abs(dpll.Acc) < 10)
//			dpll.Acc = dpll.Acc/2.0f;
//	dpll_->Acc = 200*sign(dpll_->dAc[pos] + dpll.shift);
	/* ���������� ��������� �������� ��������� ������� */
	dpll.Phi = dpll_LoopFilter(dpll_->Acc/10.0f, NULL, NULL, 2);

	dpll.Phi += ((dpll.Acc - (float)dpll.shift) / 10.0f);
//	dpll.Phi += (dpll.Acc / 100.0f);

	/**************************/

	return dpll_->Phi;
}
/** @} */

/**
 * \defgroup LoopFilter �������� ������
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
 * �������, ����������� �������� ������
 * ������� order, ����������� �� �������������
 * ����������� �������������� ���������� �����������.
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
 * ������� dpll_ClearFilt �������� ��������,
 * ���������� � �����������.
 */
void dpll_ClearFilt()
{
	dpll.dPhi = 0.0f;
}

/**
 * ������� dpll_UpdFlg ���������� ��������
 * ����� ���������� ������ �������� ���������.
 */
uint8_t dpll_UpdFlg()
{
	return dpll.updflg;
}

/**
 * ������� dpll_SetUpdFlg ������������� ��������
 * ����� ���������� ������ �������� ��������� � 1.
 */
void dpll_SetUpdFlg()
{
	dpll.updflg = 1u;
}

/**
 * ������� dpll_ClearUpdFlg ���������� ��������
 * ����� ���������� ������ �������� ��������� � 0.
 */
void dpll_ClearUpdFlg()
{
	dpll.updflg = 0u;
}

/**
 * ������� dpll_ClearAcc �������� ��������
 * �������� ���������.
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
 * ������� dpll_ClearPhi �������� �������� ��������
 * ��������� �������.
 */
void dpll_ClearPhi()
{
	dpll.Phi = 0.0f;
}

/**
 * ������� dpll_Update �������� �������� ����������� ���,
 * ����������� ������� ����.
 */
void dpll_Update()
{
	static int32_t pll_timeout;
	static uint8_t iv;

	if (dpll.intr[0] == 0) // �������� ������� ������� �� �����
	{
		/* ������������ */
		dpll.T0--;
		if(dpll.T0 > (DPLL_T_MAX + (DPLL_T_MIN - DPLL_T_MAX)/3))
			dpll.T0 -= 2;
		if(dpll.T0 > (DPLL_T_MAX + 2*(DPLL_T_MIN - DPLL_T_MAX)/3))
			dpll.T0 -= 4;

		LED_Blink(LED1);

		if(dpll.T0 < preset->Tmax) // �������� ������ ���������� ������������
		{
			dpll.T0 = preset->Tmin;
		}

		/*****************/

		dpll_ClearPhi(); // ��������� ������ ��������� �������

		dpll_ClearAcc(); // ��������� �������� ���������

		dpll_ClearFilt(); // ����� �������� ��������� �������

		dpll.ld = 0;

		iv = 0;

		pll_timeout = 0;
	}
	else
	{
		dpll.intr[0] = 0;
		LED_On(LED1);
#ifdef AGC_ON
		if((preset->termo_src == Amplitude) && (iv++ > 100u))
		{
			agc();
			iv--;
		}
#endif
		if(pll_timeout++ == 0)
		{
			pll_timeout = 0;
			dpll_Filt(preset->dpll); // ���������� ��������� �������� ��������� �������
		}

		dpll.ld = 1;
	}

	dpll.T = dpll.Phi + (float)dpll.T0; // ���������� �������� ������� ��������� �������

	/* �������� ������������ ��������� ������� ��������� ������� ������ */
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

	/* ���������� ���������� ������� �� ������� ����������� ���� */
	MDR_TIMER1->ARR = lroundf(dpll.T); // ���������� ��������� �����

	MDR_TIMER1->CCR1 = (MDR_TIMER1->ARR >> 1) + (MDR_TIMER1->ARR & 0x1); // ���������� �������� ������� CCR1

#if SCH_TYPE == 2

	MDR_TIMER1->CCR2 = MDR_TIMER1->CCR1 - (preset->att * MDR_TIMER1->ARR)/360; // ���������� �������� ������� CCR2
	MDR_TIMER1->CCR21 = MDR_TIMER1->ARR - (preset->att * MDR_TIMER1->ARR)/360; // ���������� �������� ������� CCR21
#endif
}

/**
 * \file
 * agc.c
 *
 *  ���� �������� ������� ��� ����������
 *  �������������� ����������� ��������, �
 *  ��� �� ��� ������ ��������� �������� �������
 *  � ���������� ������������ �������� ����������
 *  �� ������ ������������ �� �������� �����������.
 */


#include "agc.h"
#include "adc.h"
#include <stdlib.h>


static Preset_t * preset;

static const int32_t D = 10;

/**
 * ������������� ���.
 */
void agc_Init(Preset_t * _preset)
{
	preset = _preset;
}

/**
 * ���
 */
void agc()
{
#ifdef AGC_ON
	int16_t sup;

	sup = (int16_t)(preset->amp >> D) - preset->agc_th;

	if(abs(sup) > 20)
		preset->att = ((preset->att << D) + sup) >> D;
#endif
}

/**
 * ��������� ������ ������������ ���
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
 *  \brief �������� ��������� �������� �������
 *  ������ ������� ������������ �� ������ ��� ������
 *  ��������� ������� �� ������ �������, �� � ���
 *  ���������� �������� ���������� ������������ ���
 *  �������� �����������.
 */
int32_t agc_Amp()
{
	int32_t amp = 0;

//	ADC_SetChan(Amplitude);

	ADC_Read((uint16_t * )&amp);

	amp = abs(ADC_MEAN - amp);
#ifdef INTERFACE_TYPE_APP
	preset->pack->amp = (990 * preset->pack->amp + 10 * amp)/1000;

	return preset->pack->amp;
#endif
#ifdef INTERFACE_TYPE_MATLAB

	preset->amp = ((((1 << D) - 1) * preset->amp) >> D) + amp;

	return preset->amp;
#endif
}

/**
 * \file
 * termo.c
 *
 */


#include "termo.h"
#include "MDR32F9Qx_port.h"
#include "adc.h"

static Preset_t * _preset;

void termo_Init(Preset_t * preset)
{
	_preset = preset;
}

/**
 * ��������� ���������� ������� �����������
 */
uint16_t termo_Val()
{
	uint16_t t;

//	ADC_SetChan(_preset->termo_src);

	ADC_Read(&t);

	return t;
}

/**
 * ����� ������������� ������� �����������.
 * chan: 0 - ������ �����������, ���������� � ��;
 * 		 1 - ������ ����������� ������ ��.
 */
void termo_SetChan(uint8_t chan)
{
	if(chan == Termo_Ext)
	{
		ADC_SetChannel(TERMO_EXT_CHAN);
	}
	else if(chan == Termo_Int)
	{
		ADC_SetChannel(TERMO_INT_CHAN);
	}
	else
	{
		ADC_SetChannel(TERMO_EXT_CHAN);
	}
}

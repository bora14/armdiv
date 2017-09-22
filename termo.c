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
 * Выбор используемого датчика температуры.
 * chan: 0 - датчик температуры, встроенный в МК;
 * 		 1 - датчик температуры модуля МД.
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

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

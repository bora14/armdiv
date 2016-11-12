/**
 * \file
 * adc.c
 *
 *  Created on: 16.05.2016
 *      Author: Сламапа
 */


#include "adc.h"

void ADC_Configure()
{
	ADC_InitTypeDef sADC;
	ADCx_InitTypeDef sADCx;

	/* АЦП */
	ADC_DeInit();

	ADC_StructInit(&sADC);

	sADC.ADC_SynchronousMode      = ADC_SyncMode_Independent;
	sADC.ADC_StartDelay           = 0;
	sADC.ADC_TempSensor           = ADC_TEMP_SENSOR_Enable;
	sADC.ADC_TempSensorAmplifier  = ADC_TEMP_SENSOR_AMPLIFIER_Enable;
	sADC.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;
	sADC.ADC_IntVRefConversion    = ADC_VREF_CONVERSION_Disable;
	sADC.ADC_IntVRefTrimming      = 1;
	ADC_Init (&sADC);

	/* ADC1 Configuration */
	ADCx_StructInit (&sADCx);
	sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_CPU;
	sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_CICLIC_CONV;
	sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
	sADCx.ADC_ChannelNumber    = TERMO_EXT_CHAN;
	sADCx.ADC_Channels         = TERMO_EXT_CHAN_MSK | ADC_CH_TEMP_SENSOR_MSK | AGC_CHAN_MSK;
	sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;
	sADCx.ADC_LowLevel         = 0x800;
	sADCx.ADC_HighLevel        = 0x900;
	sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;
	sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_EXACT;
	sADCx.ADC_Prescaler        = ADC_CLK_div_8;
	sADCx.ADC_DelayGo          = 0xF;
	ADC1_Init (&sADCx);

	/* ADC1 enable */
	ADC1_Cmd(ENABLE);
}

/**
 * Установка канала АЦП для преобразования.
 */
void ADC_SetChan(adcChannel_t chan)
{
	if(chan == Termo_Ext)
	{
		ADC_SetChannel(TERMO_EXT_CHAN);
	}
	else if(chan == Termo_Int)
	{
		ADC_SetChannel(TERMO_INT_CHAN);
	}
	else if(chan == Amplitude)
	{
		ADC_SetChannel(AGC_CHAN);
	}
}


/**
 * Оцифровка напряжения на входе АЦП.
 * Переменная t должна быть 16-разрядной, так как старшие 16 разрядов
 * 32-разрядного значения, возвращаемого функцией ADC_GetResult, содержат мусор.
 */
void ADC_Read(uint16_t * t)
{
	while(ADC_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) == RESET);

	*t = ADC_GetResult();
}

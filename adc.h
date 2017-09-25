/**
 * \file
 * adc.h
 * Файл содержит функции для работы со встроенным АЦП.
 */

#ifndef ADC_H_
#define ADC_H_

#include "board.h"
#include "MDR32F9Qx_adc.h"

#define ADC_RESOL			12
#define TERMO_EXT_CHAN 		ADC_CH_ADC7
#define TERMO_EXT_CHAN_MSK 	ADC_CH_ADC7_MSK
#define TERMO_INT_CHAN 		ADC_CH_TEMP_SENSOR
#define TERMO_INT_CHAN_MSK 	ADC_CH_TEMP_SENSOR_MSK
#define AGC_CHAN			ADC_CH_ADC6
#define AGC_CHAN_MSK		ADC_CH_ADC6_MSK

#define ADC_MEAN	(1 << 11)

#define ADC_GetFlagStatus(flag)		ADC1_GetFlagStatus(flag)
#define ADC_GetResult()				ADC1_GetResult()
#define ADC_SetChannel(chan)		ADC1_SetChannel(chan)
#define ADC_Start()					ADC1_Start()

/**
 * Канал АЦП
 * \defgroup ADC_Channel ADC_Channel
 * @{
 */
typedef enum
{
	Termo_Int = 0,	///< Термодиод МК
	Termo_Ext = 1,	///< Термодиод МДГ
	Amplitude = 2	///< Сигнал на выходе МДГ
}adcChannel_t;
/** @} */

void ADC_Configure();

void ADC_Read(uint16_t * t);

#endif /* ADC_H_ */

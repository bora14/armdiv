/** \file
 * agc.h
 * \breif
 * Автоматическая регулировка усиления
 * сигнала на выходе вторичной обмотки модуля МД.
 *
 */

#ifndef AGC_H_
#define AGC_H_

#include "board.h"

void agc_Init(Preset_t * preset);

void agc();

void agc_SetTH(int16_t th);

int32_t agc_Att0();

int32_t agc_Amp();

#endif /* AGC_H_ */

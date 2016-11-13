/*
 * dma.h
 *
 *  Created on: 25.08.2016
 *      Author: Ivan
 */

#ifndef DMA_H_
#define DMA_H_

#include "board.h"
#include "MDR32F9Qx_dma.h"


#define DMA_UART_TX_CHANNEL	DMA_Channel_UART2_TX

#define DMA_BUFFER_SIZE		(sizeof(pack_t))

void dma_Init(uint8_t channel, uint32_t source, uint32_t dest);

void dma_Set(uint32_t source, uint32_t dest);

uint32_t dma_GetCurrEnableState(uint8_t DMA_Channel);

#endif /* DMA_H_ */

/**
 * \file dma.c
 *
 *  Created on: 25.08.2016
 *      Author: Ivan
 */

#include "dma.h"

static DMA_ChannelInitTypeDef DMA_InitStr;
static DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;

/**
 * Ќастройка контроллера DMA.
 */
void dma_Init(uint8_t channel, uint32_t source, uint32_t dest)
{
//	NVIC_SetPriority(DMA_IRQn, DMA_EXP_PRIOR);
//	NVIC_ClearPendingIRQ(DMA_IRQn);
//	NVIC_EnableIRQ(DMA_IRQn);

	DMA_DeInit();
	DMA_StructInit(&DMA_InitStr);

	MDR_DMA->CHNL_REQ_MASK_SET = 0xFFFFFFFF;
	MDR_DMA->CHNL_ENABLE_SET = 0xFFFFFFFF;
	MDR_DMA->CHNL_USEBURST_CLR = 0xFFFFFFFF;

	DMA_PriCtrlStr.DMA_SourceBaseAddr 			= source;
	DMA_PriCtrlStr.DMA_DestBaseAddr 			= dest;
	DMA_PriCtrlStr.DMA_SourceIncSize			= DMA_SourceIncByte;
	DMA_PriCtrlStr.DMA_DestIncSize 				= DMA_DestIncNo;
	DMA_PriCtrlStr.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte;
	DMA_PriCtrlStr.DMA_Mode 					= DMA_Mode_Basic;
	DMA_PriCtrlStr.DMA_CycleSize 				= DMA_BUFFER_SIZE >> 1;
	DMA_PriCtrlStr.DMA_NumContinuous 			= DMA_Transfers_2;
	DMA_PriCtrlStr.DMA_SourceProtCtrl 			= DMA_SourcePrivileged;
	DMA_PriCtrlStr.DMA_DestProtCtrl 			= DMA_DestPrivileged;

	DMA_InitStr.DMA_PriCtrlData 				= &DMA_PriCtrlStr;
	DMA_InitStr.DMA_Priority 					= DMA_Priority_Default;
	DMA_InitStr.DMA_UseBurst 					= DMA_BurstClear;
	DMA_InitStr.DMA_SelectDataStructure 	    = DMA_CTRL_DATA_PRIMARY;

	DMA_Init(channel, &DMA_InitStr);

	MDR_SSP1->DMACR = 0;
	MDR_SSP2->DMACR = 0;
	MDR_DMA->CHNL_REQ_MASK_CLR = 1 << channel;
	MDR_DMA->CHNL_ENABLE_CLR = 1 << DMA_Channel_SSP1_TX;
	MDR_DMA->CHNL_ENABLE_CLR |= (1 << DMA_Channel_SSP1_RX);
	MDR_DMA->CHNL_ENABLE_CLR |= (1 << DMA_Channel_SSP2_TX);
	MDR_DMA->CHNL_ENABLE_CLR |= (1 << DMA_Channel_SSP2_RX);
	MDR_DMA->CHNL_ENABLE_CLR |= (1 << DMA_Channel_ADC1);
	MDR_DMA->CHNL_ENABLE_CLR |= (1 << DMA_Channel_ADC2);
}

/**
 * \brief »нициализаци¤ канала DMA.
 *
 * dma_Set устанавливает адреса источника и приемника данных
 * дл¤ передачи.
 *
 * \param channel - номер канала DMA
 * \param source - адрес источника данных
 * \param dest - адрес приемника данных
 */
void dma_Set(uint32_t source, uint32_t dest)
{
	  DMA_PriCtrlStr.DMA_SourceBaseAddr 	= source;
	  DMA_PriCtrlStr.DMA_DestBaseAddr 		= dest;
}

/**
 * ‘ункци¤, определ¤юща¤ состо¤ние канала DMA (передает / не передает)
 */
uint32_t dma_GetCurrEnableState(uint8_t DMA_Channel)
{
  DMA_CtrlDataTypeDef *ptr;
  //получаем указатель на управл¤ющую таблицу выбранного канала
  ptr = (DMA_CtrlDataTypeDef *)(MDR_DMA->CTRL_BASE_PTR + (DMA_Channel * sizeof(DMA_CtrlDataTypeDef)));

  return (ptr->DMA_Control & 0x07);
}

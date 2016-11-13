/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \file
 * This file contains the default exception handlers.
 *
 * \note
 * The exception handler has weak aliases.
 * As they are weak aliases, any function with the same name will override
 * this definition.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "exceptions.h"
#include "board.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_timer.h"
#include "uart.h"
#include "xmodem.h"
#ifdef DMA
#include "dma.h"
#endif

static Preset_t * preset;

void Intr_Init(Preset_t * preset_)
{
	preset = preset_;
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Default interrupt handler for not used irq.
 */
void IrqHandlerNotUsed(void)
{
    while(1);
}

/**
 * \brief Default NMI interrupt handler.
 */
void NMI_Handler(void)
{
    while(1);
}

/**
 * \brief Default HardFault interrupt handler.
 */
void HardFault_Handler(void)
{
    while(1);
}

/**
 * \brief Default MemManage interrupt handler.
 */
void MemManage_Handler(void)
{
    while(1);
}

/**
 * \brief Default BusFault interrupt handler.
 */
void BusFault_Handler(void)
{
    while(1);
}

/**
 * \brief Default UsageFault interrupt handler.
 */
void UsageFault_Handler(void)
{
    while(1);
}

/**
 * \brief Default SVC interrupt handler.
 */
void SVC_Handler(void)
{
    while(1);
}

/**
 * \brief Default DebugMon interrupt handler.
 */
void DebugMon_Handler(void)
{
    while(1);
}

/**
 * \brief Default PendSV interrupt handler.
 */
void PendSV_Handler(void)
{
    while(1);
}

/**
 * \brief Default SysTick interrupt handler.
 */
extern volatile uint32_t msTicks;

void SysTick_Handler(void)
{
//	msTicks++;
	while(1);
}

void CAN1_IRQHandler(void)
{
	while(1);
}

void CAN2_IRQHandler(void)
{
	while(1);
}

void USB_IRQHandler(void)
{
	while(1);
}

void DMA_IRQHandler(void)
{
#ifdef DMA
//	if(dma_GetCurrEnableState(DMA_UART_TX_CHANNEL) == 0)
//	{
		UART_DMACmd(USE_UART, UART_DMA_TXE, DISABLE);
		MDR_DMA->CHNL_ENABLE_CLR = 1 << DMA_UART_TX_CHANNEL;
		MDR_DMA->CHNL_REQ_MASK_SET = 1 << DMA_UART_TX_CHANNEL;

		preset->pack->termo = 0;
		preset->pack->T = 0;
//	}
#else
	while(1);
#endif
}


/**
 * \brief Default interrupt handler for UART1.
 */
void UART1_IRQHandler(void)
{
	while(1);
}

/**
 * \brief Default interrupt handler for UART1.
 */

/**
 * \brief Default interrupt handler for UART2.
 */

void UART2_IRQHandler(void)
{
	UART_ClearITPendingBit(USE_UART, UART_IT_RX);

	uart_SetRxComplete();
}


void     SSP1_IRQHandler(void)
{
	while(1);
}

void     I2C_IRQHandler(void)
{
	while(1);
}

void     POWER_IRQHandler(void)
{
	while(1);
}

void     WWDG_IRQHandler(void)
{
	while(1);
}

#include "dpll.h"
#include <math.h>
#include <stdlib.h>
static int8_t sweep;

void     Timer1_IRQHandler(void)
{
	if (MDR_TIMER1->STATUS & TIMER_STATUS_CCR_CAP_CH3)
	{
		MDR_TIMER1->STATUS &= ~TIMER_STATUS_CCR_CAP_CH3;

		preset->dpll->dAc[preset->dpll->cnt] = ((int32_t)MDR_TIMER1->CCR3 - (int32_t)MDR_TIMER1->CCR1);

		MDR_TIMER1->CCR3 = 0u;

		preset->dpll->intr[0] = 1;

		sweep = 0;

		dpll_SetUpdFlg();

		preset->dpll->cnt ^= 0x1;
	}

	if (MDR_TIMER1->STATUS & TIMER_STATUS_CNT_ARR)
	{

		MDR_TIMER1->STATUS &= ~TIMER_STATUS_CNT_ARR;

		if(sweep++ > preset->sweep)
		{
			sweep = 0;
			dpll_SetUpdFlg();
		}
	}
}

void     Timer2_IRQHandler(void)
{
	while(1);
}

void     Timer3_IRQHandler(void)
{
	if (MDR_TIMER3->STATUS & TIMER_STATUS_CNT_ARR)
	{
		MDR_TIMER3->STATUS &= ~TIMER_STATUS_CNT_ARR;

		if( (preset->mode != UPLOAD) && (preset->mode != DOWNLOAD))
			preset->agc_start = 1;
		else
		{
			xmodem_set_timeout();
		}
	}
}

void     ADC_IRQHandler(void)
{
	while(1);
}

void     COMPARATOR_IRQHandler(void)
{
	while(1);
}

void     SSP2_IRQHandler(void)
{
	while(1);
}

void     BACKUP_IRQHandler(void)
{
	while(1);
}

void     EXT_INT1_IRQHandler(void)
{
	while(1);
}

void     EXT_INT2_IRQHandler(void)
{
	while(1);
}

void     EXT_INT3_IRQHandler(void)
{
	while(1);
}

void     EXT_INT4_IRQHandler(void)
{
	while(1);
}


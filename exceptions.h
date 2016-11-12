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
 * Interface for default exception handlers.
 */

#ifndef _EXCEPTIONS_
#define _EXCEPTIONS_

#include "dpll.h"

/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/

/* Function prototype for exception table items (interrupt handler). */
typedef void( *IntFunc )( void );

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

void IrqHandlerNotUsed( void ) ;

void NMI_Handler( void );
void HardFault_Handler( void );
void MemManage_Handler( void );
void BusFault_Handler( void );
void UsageFault_Handler( void );
void SVC_Handler( void );
void DebugMon_Handler( void );
void PendSV_Handler( void );
void SysTick_Handler( void );

/* External Interrupts */
void     CAN1_IRQHandler(void);
void     CAN2_IRQHandler(void);
void     USB_IRQHandler(void);
void     DMA_IRQHandler(void);
void     UART1_IRQHandler(void);
void     UART2_IRQHandler(void);
void     SSP1_IRQHandler(void);
void     I2C_IRQHandler(void);
void     POWER_IRQHandler(void);
void     WWDG_IRQHandler(void);
void     Timer1_IRQHandler(void);
void     Timer2_IRQHandler(void);
void     Timer3_IRQHandler(void);
void     ADC_IRQHandler(void);
void     COMPARATOR_IRQHandler(void);
void     SSP2_IRQHandler(void);
void     BACKUP_IRQHandler(void);
void     EXT_INT1_IRQHandler(void);
void     EXT_INT2_IRQHandler(void);
void     EXT_INT3_IRQHandler(void);
void     EXT_INT4_IRQHandler(void);

void Intr_Init(Preset_t * preset);

#endif /* #ifndef _EXCEPTIONS_ */

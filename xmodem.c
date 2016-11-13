/*
 * xmodem.c
 *
 *  Created on: 13.09.2011
 *      Author: iv14
 */

#include "xmodem.h"
#include "MDR32F9Qx_timer.h"
#include "memo.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

static volatile uint8_t timeout;

/**
 * \brief Инициализация XMODEM-а
 *
 * Инициализация переферии для работы по
 * протоколу XMODEM.
 */
void xmoden_init()
{
	TIMER_CntInitTypeDef TIMER_CntInitStruct;

	/*************************************/

	TIMER_DeInit(MDR_TIMER3);

	TIMER_CntInitStruct.TIMER_IniCounter 		= 0;
	TIMER_CntInitStruct.TIMER_Prescaler 		= CPU_MCK / __UINT16_MAX__;
	TIMER_CntInitStruct.TIMER_Period     		= __UINT16_MAX__ - 1;
	TIMER_CntInitStruct.TIMER_CounterMode      	= TIMER_CntMode_ClkFixedDir;
	TIMER_CntInitStruct.TIMER_CounterDirection 	= TIMER_CntDir_Up;
	TIMER_CntInitStruct.TIMER_EventSource      	= TIMER_EvSrc_None;
	TIMER_CntInitStruct.TIMER_FilterSampling   	= TIMER_FDTS_TIMER_CLK_div_1;
	TIMER_CntInitStruct.TIMER_ARR_UpdateMode   	= TIMER_ARR_Update_On_CNT_Overflow;
	TIMER_CntInitStruct.TIMER_ETR_FilterConf   	= TIMER_Filter_8FF_at_TIMER_CLK;
	TIMER_CntInitStruct.TIMER_ETR_Prescaler    	= TIMER_ETR_Prescaler_None;
	TIMER_CntInitStruct.TIMER_ETR_Polarity     	= TIMER_ETRPolarity_NonInverted;
	TIMER_CntInitStruct.TIMER_BRK_Polarity     	= TIMER_BRKPolarity_NonInverted;

	TIMER_CntInit(MDR_TIMER3, &TIMER_CntInitStruct);
	/*************************************************/

	TIMER_BRGInit(MDR_TIMER3,TIMER_HCLKdiv1);

	TIMER_Cmd(MDR_TIMER3, ENABLE);

	MDR_TIMER3->IE = 0x0;

	TIMER_ITConfig(MDR_TIMER3, TIMER_STATUS_CNT_ARR, ENABLE);

	NVIC_SetPriority(Timer3_IRQn, TIMER3_EXP_PRIOR);

	NVIC_EnableIRQ(Timer3_IRQn);
}

uint8_t xmodem_trans(const uint8_t *data, const uint32_t len)
{
	uint16_t crc;
	uint8_t ch = 0, i = 0, num_blocks, rem,
			num_blk, len_blk, *p_data;

	len_blk = XMODEM_LEN_BLOCK;

	while((i++ < XMODEM_WAIT_NUM) && (ch != XMODEM_C))
	{
		ch = xmodem_wait_ch();
	}

	if(ch != XMODEM_C)
		return FAILURE;

	num_blocks = len / XMODEM_LEN_BLOCK;
	rem = len % XMODEM_LEN_BLOCK;
	if(rem != 0)
		num_blocks++;

	p_data = data;

	for(num_blk = 1; num_blk <= num_blocks; num_blk++)
	{
		crc = 0u
				;
		UART_PutChar(USE_UART, XMODEM_SOH);
		UART_PutChar(USE_UART, num_blk & 0xff);
		UART_PutChar(USE_UART, ~num_blk & 0xff);

		if((num_blk == num_blocks) && (rem != 0u))
		{
			len_blk = rem;
		}

		for(i = 0; i < XMODEM_LEN_BLOCK; i++)
		{
			if(i < len_blk)
			{
				crc_xmodem_update(&crc, p_data);

				UART_PutChar(USE_UART, *p_data);

				p_data++;
			}
			else
			{
				crc_xmodem_update(&crc, 0);

				UART_PutChar(USE_UART, 0);
			}
		}

		UART_PutChar(USE_UART, (crc >> 8u) & 0xff);
		UART_PutChar(USE_UART, crc & 0xff);

		ch = xmodem_wait_ch();

		if(ch != XMODEM_ACK)
			return FAILURE;
	}

	UART_PutChar(USE_UART, XMODEM_EOT);

	return SUCCESS;
}

uint8_t xmodem_recv(uint8_t * data, uint32_t len)
{
	uint8_t i = 0, ch = 0, num, num_;
	uint8_t * p_data;
	uint16_t crc = 0, crc_recv = 0;

	p_data = data;

	while((i++ < XMODEM_WAIT_NUM) && (ch != XMODEM_SOH))
	{
		UART_PutChar(USE_UART, XMODEM_C);

		ch = xmodem_wait_ch();
	}

	if(ch != XMODEM_SOH)
		return FAILURE;

	while((ch != XMODEM_EOT))// && (xmodem_get_timeout() != XMODEM_TIMEOUT_STATE_Yes))
	{
		crc = 0u;

		num = xmodem_wait_ch();
		num_ = xmodem_wait_ch();

		for(i = 0; i < XMODEM_LEN_BLOCK; i++)
		{
			ch = xmodem_wait_ch();

			crc_xmodem_update(&crc, &ch);

			if(len > 0)
			{
				*p_data = ch;
				p_data++;
				len--;
			}
		}

		crc_recv = (xmodem_wait_ch() << 8u) & 0xff00;

		crc_recv |= (xmodem_wait_ch() & 0xff);

		if((crc_recv == crc) && (num = ~num_))
		{
			UART_PutChar(USE_UART, XMODEM_ACK);
		}
		else
		{
			UART_PutChar(USE_UART, XMODEM_NAK);
			p_data -= XMODEM_LEN_BLOCK;
			len += XMODEM_LEN_BLOCK;
		}

		ch = xmodem_wait_ch();
	}

	UART_PutChar(USE_UART, XMODEM_ACK);

	if(xmodem_wait_ch() == XMODEM_ETB)
		UART_PutChar(USE_UART, XMODEM_ACK);

	return SUCCESS;
}

void xmodem_restart()
{
	TIMER_SetCounter(MDR_TIMER3, 0);
}

void xmodem_set_timeout()
{
	timeout = XMODEM_TIMEOUT_STATE_Yes;
}

void xmodem_clear_timeout()
{
	timeout = XMODEM_TIMEOUT_STATE_No;
}

uint8_t xmodem_get_timeout()
{
	return timeout;
}

uint8_t xmodem_wait_ch()
{
	uint8_t ch = 0;

	xmodem_clear_timeout();

	xmodem_restart();

	while(xmodem_get_timeout() != XMODEM_TIMEOUT_STATE_Yes)
	{
		if(uart_GetRxComplete())
		{
			uart_ClearRxComplete();

			ch = UART_GetChar(USE_UART);

			break;
		}
	}

	return ch;
}

uint16_t crc_xmodem_update(uint16_t * crc, uint8_t  * ch)
{
	uint8_t i;

	(*crc) = (*crc) ^ ((uint16_t)(*ch) << 8u);
	for (i = 0u; i < 8u; i++)
	{
		if ((*crc) & 0x8000)
			(*crc) = ((*crc) << 1u) ^ 0x1021;
		else
			(*crc) <<= 1;
	}

	return *crc;
}

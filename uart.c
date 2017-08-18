/*
 * uart.c
 *
 *  Created on: 13.03.2013
 *      Author: sokolovim
 */
#include "uart.h"

#include <stdarg.h>
#include <assert.h>
#include <string.h>

static uart_cmd_t uart_cmd;

static uint8_t uart_RxFlg;

void UART_Configure(uint32_t baud)
{
	UART_InitTypeDef UART_InitType;

	UART_BRGInit(USE_UART, UART_HCLKdiv1);

	UART_StructInit(&UART_InitType);

	UART_InitType.UART_BaudRate = baud;

#ifdef UART_FIFO_TR
	UART_InitType.UART_FIFOMode = UART_FIFO_ON;

	USE_UART->IFLS = (0 << UART_IFLS_RXIFLSEL_Pos) | (4 << UART_IFLS_TXIFLSEL_Pos);
#endif

	UART_Init(USE_UART, &UART_InitType);

#ifdef DMA
	UART_DMAConfig(USE_UART, UART_IT_FIFO_LVL_12words, UART_IT_FIFO_LVL_12words);

	UART_DMACmd(USE_UART,(UART_DMA_RXE | UART_DMA_TXE | UART_DMA_ONERR), ENABLE);
#endif

	NVIC_EnableIRQ(UART2_IRQn);

	UART_ITConfig(USE_UART, UART_IT_RT | UART_IT_RX, ENABLE);

	UART_Cmd(USE_UART, ENABLE);
}

uint16_t UART_GetChar(MDR_UART_TypeDef* UARTx)
{
	uint16_t ch;
	while(UART_GetFlagStatus(UARTx, UART_FLAG_BUSY) == SET);
	ch = UART_ReceiveData(UARTx);
	return ch;
}

uint16_t UART_Read(MDR_UART_TypeDef* UARTx, uint32_t timeOut)
{
	if (timeOut == 0)
	{
		while(UART_GetFlagStatus(UARTx, UART_FLAG_RXFF) == RESET);
	}
	else
	{
		while (UART_GetFlagStatus(UARTx, UART_FLAG_RXFF) == RESET)
		{

			if (timeOut == 0)
			{
				return 0;
			}
			timeOut--;
		}
	}
	/* Check the parameters */
	assert_param(IS_UART_ALL_PERIPH(UARTx));

	/* Receive Data */
	return (uint16_t)(UARTx->DR);
}

void uart_put_string (MDR_UART_TypeDef* uart, uint8_t *data_string)
{
	while(*data_string) UART_PutChar (uart, *data_string++);
}

//------------------------------------------------------------------------------
//  @fn uart_mini_printf
//!
//! Minimal "PRINTF" with variable argument list. Write several variables
//! formatted by a format string to a file descriptor.
//! Example:
//! ========
//! { u8_toto = 0xAA;
//!   uart_mini_printf ("toto = %04d (0x%012X)\r\n", u8_toto, u8_toto);
//!   /*   Expected:     toto = 0170 (0x0000000000AA)   &  Cr+Lf       */ }
//!
//! @warning "uart_init()" must be performed before
//!
//! @param argument list
//!
//!     The format string is interpreted like this:
//!        ,---------------,---------------------------------------------------,
//!        | Any character | Output as is                                      |
//!        |---------------+---------------------------------------------------|
//!        |     %c:       | interpret argument as character                   |
//!        |     %s:       | interpret argument as pointer to string           |
//!        |     %d:       | interpret argument as decimal (signed) S16        |
//!        |     %ld:      | interpret argument as decimal (signed) S32        |
//!        |     %u:       | interpret argument as decimal (unsigned) U16      |
//!        |     %lu:      | interpret argument as decimal (unsigned) U32      |
//!        |     %x:       | interpret argument as hex U16 (lower case chars)  |
//!        |     %lx:      | interpret argument as hex U32 (lower case chars)  |
//!        |     %X:       | interpret argument as hex U16 (upper case chars)  |
//!        |     %lX:      | interpret argument as hex U32 (upper case chars)  |
//!        |     %%:       | print a percent ('%') character                   |
//!        '---------------'---------------------------------------------------'
//!
//!     Field width (in decimal) always starts with "0" and its maximum is
//!     given by "UART_DATA_BUF_LEN" defined in "uart_lib.h".
//!        ,----------------------,-----------,--------------,-----------------,
//!        |       Variable       | Writting  |  Printing    |    Comment      |
//!        |----------------------+-----------+--------------|-----------------|
//!        |                      |   %x      | aa           |        -        |
//!        |  u8_xx = 0xAA        |   %04d    | 0170         |        -        |
//!        |                      |   %012X   | 0000000000AA |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        | u16_xx = -5678       |   %010d   | -0000005678  |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        | u32_xx = -4100000000 |   %011lu  | 00194967296  |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        |          -           |   %8x     | 8x           | Writting error! |
//!        |----------------------+-----------+--------------|-----------------|
//!        |          -           |   %0s     | 0s           | Writting error! |
//!        '----------------------'-----------'--------------'-----------------'
//!
//! Return: 0 = O.K.
//!
//------------------------------------------------------------------------------
uint8_t uart_mini_printf(MDR_UART_TypeDef* uart, char *format, ...)
{
	va_list arg_ptr;
	uint8_t      *p,*sval;
	uint8_t      u8_temp, n_sign, data_idx, min_size;
	uint8_t      data_buf[UART_DATA_BUF_LEN];
	int8_t      long_flag, alt_p_c;
	int8_t      s8_val;
	int16_t     s16_val;
	int32_t     s32_val;
	uint16_t     u16_val;
	uint32_t     u32_val;

	long_flag = FALSE;
	alt_p_c = FALSE;
	min_size = UART_DATA_BUF_LEN-1;

	while(UART_GetFlagStatus(uart, UART_FLAG_TXFE) != SET);

	va_start(arg_ptr, format);   // make arg_ptr point to the first unnamed arg
	for (p = (uint8_t *) format; *p; p++)
	{
		if ((*p == '%') || (alt_p_c == TRUE))
		{
			p++;
		}
		else
		{
			UART_PutChar(uart, *p);
			alt_p_c = FALSE;
			long_flag = FALSE;
			continue;   // "switch (*p)" section skipped
		}
		switch (*p)
		{
		case 'c':
			if (long_flag == TRUE)      // ERROR: 'l' before any 'c'
			{
				UART_PutChar(uart, 'l');
				UART_PutChar(uart, 'c');
			}
			else
			{
				s8_val = (int8_t)(va_arg(arg_ptr, int));    // s8_val = (int8_t)(va_arg(arg_ptr, int16_t));
				UART_PutChar(uart, (uint8_t)(s8_val));
			}
			// Clean up
			min_size = UART_DATA_BUF_LEN-1;
			alt_p_c = FALSE;
			long_flag = FALSE;
			break; // case 'c'

		case 's':
			if (long_flag == TRUE)      // ERROR: 'l' before any 's'
			{
				UART_PutChar(uart, 'l');
				UART_PutChar(uart, 's');
			}
			else
			{
				for (sval = va_arg(arg_ptr, uint8_t *); *sval; sval++)
				{
					UART_PutChar(uart, *sval);
				}
			}
			// Clean up
			min_size = UART_DATA_BUF_LEN-1;
			alt_p_c = FALSE;
			long_flag = FALSE;
			break;  // case 's'

		case 'l':  // It is not the number "ONE" but the lower case of "L" character
			if (long_flag == TRUE)      // ERROR: two consecutive 'l'
			{
				UART_PutChar(uart, 'l');
				alt_p_c = FALSE;
				long_flag = FALSE;
			}
			else
			{
				alt_p_c = TRUE;
				long_flag = TRUE;
			}
			p--;
			break;  // case 'l'

		case 'd':
			n_sign  = FALSE;
			for(data_idx = 0; data_idx < (UART_DATA_BUF_LEN-1); data_idx++)
			{
				data_buf[data_idx] = '0';
			}
			data_buf[UART_DATA_BUF_LEN-1] = 0;
			data_idx = UART_DATA_BUF_LEN - 2;
			if (long_flag)  // 32-bit
			{
				s32_val = va_arg(arg_ptr, int32_t);
				if (s32_val < 0)
				{
					n_sign = TRUE;
					s32_val  = -s32_val;
				}
				while (1)
				{
					data_buf[data_idx] = s32_val % 10 + '0';
					s32_val /= 10;
					data_idx--;
					if (s32_val==0) break;
				}
			}
			else  // 16-bit
			{
				s16_val = (int16_t)(va_arg(arg_ptr, int)); // s16_val = va_arg(arg_ptr, int16_t);
				if (s16_val < 0)
				{
					n_sign = TRUE;
					s16_val  = -s16_val;
				}
				while (1)
				{
					data_buf[data_idx] = s16_val % 10 + '0';
					s16_val /= 10;
					data_idx--;
					if (s16_val==0) break;
				}
			}
			if (n_sign) { UART_PutChar(uart, '-'); }
			data_idx++;
			if (min_size < data_idx)
			{
				data_idx = min_size;
			}
			uart_put_string (uart, data_buf + data_idx);
			// Clean up
			min_size = UART_DATA_BUF_LEN-1;
			alt_p_c = FALSE;
			long_flag = FALSE;
			break;  // case 'd'

		case 'u':
			for(data_idx = 0; data_idx < (UART_DATA_BUF_LEN-1); data_idx++)
			{
				data_buf[data_idx] = '0';
			}
			data_buf[UART_DATA_BUF_LEN-1] = 0;
			data_idx = UART_DATA_BUF_LEN - 2;
			if (long_flag)  // 32-bit
			{
				u32_val = va_arg(arg_ptr, uint32_t);
				while (1)
				{
					data_buf[data_idx] = u32_val % 10 + '0';
					u32_val /= 10;
					data_idx--;
					if (u32_val==0) break;
				}
			}
			else  // 16-bit
			{
				u16_val = (uint16_t)(va_arg(arg_ptr, int)); // u16_val = va_arg(arg_ptr, uint16_t);
				while (1)
				{
					data_buf[data_idx] = u16_val % 10 + '0';
					data_idx--;
					u16_val /= 10;
					if (u16_val==0) break;
				}
			}
			data_idx++;
			if (min_size < data_idx)
			{
				data_idx = min_size;
			}
			uart_put_string (uart, data_buf + data_idx);
			// Clean up
			min_size = UART_DATA_BUF_LEN-1;
			alt_p_c = FALSE;
			long_flag = FALSE;
			break;  // case 'u':

		case 'x':
		case 'X':
			for(data_idx = 0; data_idx < (UART_DATA_BUF_LEN-1); data_idx++)
			{
				data_buf[data_idx] = '0';
			}
			data_buf[UART_DATA_BUF_LEN-1] = 0;
			data_idx = UART_DATA_BUF_LEN - 2;
			if (long_flag)  // 32-bit
			{
				u32_val = va_arg(arg_ptr, uint32_t);
				while (u32_val)
				{
					u8_temp = (uint8_t)(u32_val & 0x0F);
					data_buf[data_idx] = (u8_temp < 10)? u8_temp+'0':u8_temp-10+(*p=='x'?'a':'A');
					u32_val >>= 4;
					data_idx--;
				}
			}
			else  // 16-bit
			{
				u16_val = (uint16_t)(va_arg(arg_ptr, int)); // u16_val = va_arg(arg_ptr, uint16_t);
				while (u16_val)
				{
					u8_temp = (uint8_t)(u16_val & 0x0F);
					data_buf[data_idx] = (u8_temp < 10)? u8_temp+'0':u8_temp-10+(*p=='x'?'a':'A');
					u16_val >>= 4;
					data_idx--;
				}
			}
			data_idx++;
			if (min_size < data_idx)
			{
				data_idx = min_size;
			}
			uart_put_string (uart, data_buf + data_idx);
			// Clean up
			min_size = UART_DATA_BUF_LEN-1;
			alt_p_c = FALSE;
			long_flag = FALSE;
			break;  // case 'x' & 'X'

		case '0':   // Max allowed "min_size" 2 decimal digit, truncated to UART_DATA_BUF_LEN-1.
			min_size = UART_DATA_BUF_LEN-1;
			if (long_flag == TRUE)      // ERROR: 'l' before '0'
			{
				UART_PutChar(uart, 'l');
				UART_PutChar(uart, '0');
				// Clean up
				alt_p_c = FALSE;
				long_flag = FALSE;
				break;
			}
			u8_temp = *++p;
			if ((u8_temp >='0') && (u8_temp <='9'))
			{
				min_size = u8_temp & 0x0F;
				u8_temp = *++p;
				if ((u8_temp >='0') && (u8_temp <='9'))
				{
					min_size <<= 4;
					min_size |= (u8_temp & 0x0F);
					p++;
				}
				min_size = ((min_size & 0x0F) + ((min_size >> 4) *10));  // Decimal to hexa
				if (min_size > (UART_DATA_BUF_LEN-1))
				{
					min_size = (UART_DATA_BUF_LEN-1);
				}  // Truncation
				min_size = UART_DATA_BUF_LEN-1 - min_size;  // "min_size" formatted as "data_ix"
			}
			else      // ERROR: any "char" after '0'
			{
				UART_PutChar(uart, '0');
				UART_PutChar(uart, *p);
				// Clean up
				alt_p_c = FALSE;
				long_flag = FALSE;
				break;
			}
			p-=2;
			alt_p_c = TRUE;
			// Clean up
			long_flag = FALSE;
			break;  // case '0'

		default:
			if (long_flag == TRUE)
			{
				UART_PutChar(uart, 'l');
			}
			UART_PutChar(uart, *p);
			// Clean up
			min_size = UART_DATA_BUF_LEN-1;
			alt_p_c = FALSE;
			long_flag = FALSE;
			break;  // default

		}   // switch (*p ...

	}   // for (p = ...

	va_end(arg_ptr);
	return 0;
}

void uart_SetRxComplete()
{
	uart_RxFlg = 1u;
}

uint8_t uart_GetRxComplete()
{
	return uart_RxFlg;
}

void uart_ClearRxComplete()
{
	uart_RxFlg = 0u;
}

uart_cmd_t * uart_Cmd()
{
	return &uart_cmd;
}

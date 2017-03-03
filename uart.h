/** \file
 * uart.h
 *
 *  Created on: 13.03.2013
 *      Author: sokolovim
 */

#ifndef UART_H_
#define UART_H_

#include "MDR32F9Qx_uart.h"
#include "board.h"

#define UART_DATA_BUF_LEN (128)
#define UART_MARK_LEN	(2)
#define FALSE (1==0)
#define TRUE (1==1)
#define UART_TIMEOUT	(10)

#define CONF_8BIT_NOPAR_1STOP      ( (UART_MR_PAR_NO) | (UART_MR_CHMODE_NORMAL) )
#define CONF_8BIT_EVENPAR_1STOP    ( (UART_MR_PAR_EVEN) | (UART_MR_CHMODE_NORMAL) )
#define CONF_8BIT_ODDPAR_1STOP     ( (UART_MR_PAR_ODD) | (UART_MR_CHMODE_NORMAL) )

#define UART_Receiver_En		(1)
#define UART_Receiver_Dis		(0)
#define UART_Transmitter_En		(1)
#define UART_Transmitter_Dis	(0)

#ifndef UART_FIFO_TR
#define UART_PutChar(uart, ch)	{while(UART_GetFlagStatus(uart, UART_FLAG_BUSY) == SET); UART_SendData(uart,ch);}
#else
#define UART_PutChar(uart, ch)	{UART_SendData(uart,ch);}
#endif

#ifdef __cplusplus
 extern "C" {
#endif

 typedef union
 {
 	uint16_t Mrk;
 	uint8_t data[UART_DATA_BUF_LEN];
 }uart_cmd_t;

void UART_Configure(uint32_t baud);

uint16_t UART_GetChar(MDR_UART_TypeDef* UARTx);

uint16_t UART_Read(MDR_UART_TypeDef* UARTx, uint32_t timeOut);

void uart_put_string (MDR_UART_TypeDef* uart, uint8_t *data_string);

uint8_t uart_mini_printf(MDR_UART_TypeDef* uart, char *format, ...);

void uart_SetRxComplete();

uint8_t uart_GetRxComplete();

void uart_ClearRxComplete();

#ifdef __cplusplus
}
#endif

#endif /* UART_H_ */

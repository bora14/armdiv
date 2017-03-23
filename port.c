/*
 * port.c
 *
 *  Created on: 04.02.2016
 *      Author: Work
 */

#include "port.h"

void Port_Configure()
{
	PORT_InitTypeDef Port;

	PORT_DeInit(MDR_PORTA);
	PORT_DeInit(MDR_PORTB);
	PORT_DeInit(MDR_PORTC);
	PORT_DeInit(MDR_PORTD);
	PORT_DeInit(MDR_PORTE);
	PORT_DeInit(MDR_PORTF);

	/*************************************/
	/* DPLL  */
	/*************************************/

	PORT_StructInit(&Port);
	// OUT
	Port.PORT_Pin = PORT_Pin_1;
	Port.PORT_OE = PORT_OE_OUT;
	Port.PORT_MODE = PORT_MODE_DIGITAL;
	Port.PORT_FUNC = PORT_FUNC_ALTER;
	Port.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init(MDR_PORTA, &Port);

#if SCH_TYPE == 2
	// OUT
	Port.PORT_Pin = PORT_Pin_3;
	Port.PORT_OE = PORT_OE_OUT;
	Port.PORT_MODE = PORT_MODE_DIGITAL;
	Port.PORT_FUNC = PORT_FUNC_ALTER;
	Port.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init(MDR_PORTA, &Port);
#endif

	// IN
	Port.PORT_Pin = PORT_Pin_5;
	Port.PORT_OE = PORT_OE_IN;
	Port.PORT_MODE = PORT_MODE_DIGITAL;
	Port.PORT_FUNC = PORT_FUNC_ALTER;
	Port.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &Port);

	/*************************************/
	/* Amplitude control */
	/*************************************/
	PORT_StructInit(&Port);

	Port.PORT_Pin = PORT_Pin_5;
	Port.PORT_OE = PORT_OE_OUT;
	Port.PORT_MODE = PORT_MODE_DIGITAL;
	Port.PORT_FUNC = PORT_FUNC_OVERRID;
	Port.PORT_SPEED = PORT_SPEED_FAST;

	PORT_Init(MDR_PORTB, &Port);
	/*************************************/
	/* UART */
	/*************************************/
	PORT_StructInit(&Port);

	Port.PORT_PULL_UP = PORT_PULL_UP_ON;
	Port.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	Port.PORT_PD_SHM = PORT_PD_SHM_OFF;
	Port.PORT_PD = PORT_PD_DRIVER;
	Port.PORT_GFEN = PORT_GFEN_OFF;
	Port.PORT_FUNC = PORT_FUNC_OVERRID;
	Port.PORT_SPEED = PORT_SPEED_MAXFAST;
	Port.PORT_MODE = PORT_MODE_DIGITAL;

	/* Configure PORTF pins 1 (UART2_TX) as output */
	Port.PORT_OE = PORT_OE_OUT;
	Port.PORT_Pin = PORT_Pin_1;
	PORT_Init(MDR_PORTF, &Port);

	/* Configure PORTF pins 0 (UART2_RX) as input */
	Port.PORT_OE = PORT_OE_IN;
	Port.PORT_Pin = PORT_Pin_0;
	PORT_Init(MDR_PORTF, &Port);
//#endif
	/*************************************/
	/* I2C for MEM  */
	/*************************************/
	PORT_StructInit(&Port);

	Port.PORT_PULL_UP = PORT_PULL_UP_ON;
	Port.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	Port.PORT_PD_SHM = PORT_PD_SHM_OFF;
	Port.PORT_PD = PORT_PD_DRIVER;
	Port.PORT_GFEN = PORT_GFEN_OFF;
	Port.PORT_FUNC = PORT_FUNC_ALTER;
	Port.PORT_SPEED = PORT_SPEED_MAXFAST;
	Port.PORT_MODE = PORT_MODE_DIGITAL;

	/* Configure PORTC pins 0,1 (I2C_SCL,I2C_SDA) */
	Port.PORT_Pin = PORT_Pin_0 | PORT_Pin_1;
	PORT_Init(MDR_PORTC, &Port);

	/*************************************/
	/* ADC for Termo */
	/*************************************/
	PORT_StructInit(&Port);

	Port.PORT_Pin  = PORT_Pin_7;
	Port.PORT_OE   = PORT_OE_IN;
	Port.PORT_MODE = PORT_MODE_ANALOG;

	PORT_Init(MDR_PORTD, &Port);

	/*************************************/
	/* ADC for AGC */
	/*************************************/
	PORT_StructInit(&Port);

	Port.PORT_Pin  = PORT_Pin_6;
	Port.PORT_OE   = PORT_OE_IN;
	Port.PORT_MODE = PORT_MODE_ANALOG;

	PORT_Init(MDR_PORTD, &Port);

	/*************************************/
	/* LED */
	/*************************************/
	PORT_StructInit(&Port);

	Port.PORT_Pin = LED1;
	Port.PORT_OE = PORT_OE_OUT;
	Port.PORT_MODE = PORT_MODE_DIGITAL;
	Port.PORT_FUNC = PORT_FUNC_PORT;
	Port.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(LED_PORT, &Port);

#ifdef OSC_TEST
	Port.PORT_Pin = PORT_Pin_0;
	Port.PORT_OE = PORT_OE_OUT;
	Port.PORT_MODE = PORT_MODE_DIGITAL;
	Port.PORT_FUNC = PORT_FUNC_ALTER;
	Port.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(MDR_PORTB, &Port);
#endif
}

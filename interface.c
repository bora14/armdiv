﻿/*
 * interface.c
 *
 *  Created on: 18.01.2016
 *      Author: �������
 */

#include "interface.h"
#include "timer.h"
#include "dpll.h"
#include "termo.h"
#include "adc.h"
#include "ta.h"
#include "stdlib.h"
#ifdef DMA
#include "dma.h"
#endif
#include "agc.h"

#if defined(INTERFACE_TYPE_MATLAB)
static pack_t pack = {SOT, 0, 0, EOT};
#endif
#if defined(INTERFACE_TYPE_APP)
static pack_t pack = {SOT, 0, 0, 0, 0.0f, 0, 0};
#endif

static uart_cmd_t  uart_cmd;
static Preset_t * preset;

void interface_Init(Preset_t * _preset)
{
	preset = _preset;

	preset->pack = &pack;

	UART_Configure(UART_WORK_BAUD);
}

void dataTr()
{
#ifdef DMA
	dma_Init(DMA_UART_TX_CHANNEL, (uint32_t) preset->pack, (uint32_t) &USE_UART->DR);

	DMA_Cmd(DMA_UART_TX_CHANNEL, ENABLE);

	UART_DMACmd(USE_UART, UART_DMA_TXE, ENABLE);
#else
	int32_t i;
	uint8_t * p;

	p = (uint8_t *)&pack;

	for(i = 0; i < MESSAGE_LEN; i++)
	{
		UART_PutChar(USE_UART, (uint16_t) *p++);
	}
#ifndef AGC_RECU
	pack.termo = 0;
#endif

#endif
}

pack_t * get_Pack()
{
	return &pack;
}

void dataRcv()
{
	int32_t arg;

	arg = atol((char *)&uart_cmd.data[UART_MARK_LEN]);

	switch(uart_cmd.Mrk)
	{
#ifdef NUMBER_VERSION
	case LS:
		uart_mini_printf(USE_UART, "\r\n NUMBER_VERSION %s \r\n", NUMBER_VERSION);
		uart_mini_printf(USE_UART, "\r\n sweep %ld \r\n", preset->sweep);
		uart_mini_printf(USE_UART, "\r\n ave_num %ld \r\n", preset->ave_num);
		uart_mini_printf(USE_UART, "\r\n Tmax %ld \r\n", preset->Tmax);
		uart_mini_printf(USE_UART, "\r\n Tmin %ld \r\n", preset->Tmin);
		uart_mini_printf(USE_UART, "\r\n sens_num %d \r\n", preset->sens_num);
		uart_mini_printf(USE_UART, "\r\n freq %ld \r\n", preset->freq);
#if SCH_TYPE == 1
		uart_mini_printf(USE_UART, "\r\n duty_cycle %ld \r\n", preset->duty_cycle);
#elif SCH_TYPE == 2
		uart_mini_printf(USE_UART, "\r\n att %d \r\n", preset->att);
		uart_mini_printf(USE_UART, "\r\n att0 %d \r\n", preset->att0);
		uart_mini_printf(USE_UART, "\r\n agc_on %c \r\n", preset->agc_on);
#endif
#ifdef AGC_ON
		uart_mini_printf(USE_UART, "\r\n agc_th %d \r\n", preset->agc_th);
#endif
		uart_mini_printf(USE_UART, "\r\n shift %ld \r\n", preset->shift);
		uart_mini_printf(USE_UART, "\r\n edge %d \r\n", preset->edge);
		uart_mini_printf(USE_UART, "\r\n mode %d \r\n", preset->mode);
		uart_mini_printf(USE_UART, "\r\n termo_src %d \r\n", preset->termo_src);
		break;
#endif
	case UP:
		if(arg > DPLL_F_MAX)
		{
			uart_mini_printf(USE_UART, CMD_OUT_OF_RANGE);
		}
		else
		{
			preset->Tmax = CPU_MCK/((TIMER1_Prescaler + 1u) * arg);
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
		break;
	case DN:
		if(arg < DPLL_F_MIN)
		{
			uart_mini_printf(USE_UART, CMD_OUT_OF_RANGE);
		}
		else
		{
			preset->Tmin = CPU_MCK/((TIMER1_Prescaler + 1u) * arg);
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
		break;
	case AM:
#if SCH_TYPE == 1
		if((arg < PWR_CTRL_PERIOD_MIN) || (arg > PWR_CTRL_PERIOD_MAX))
		{
			uart_mini_printf(USE_UART, CMD_OUT_OF_RANGE);
		}
		else
		{
			preset->duty_cycle = arg;
			AMP_Ctrl(arg);
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
#endif
#if SCH_TYPE == 2
		if(arg > 179) /* �� ����� 179 ��������*/
		{
			uart_mini_printf(USE_UART, CMD_OUT_OF_RANGE);
		}
		else
		{
			AMP_Ctrl((int16_t)arg);
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
#endif
		break;
	case SW:
		if((arg < MIN_SWEEP) || (arg > MAX_SWEEP))
		{
			uart_mini_printf(USE_UART, CMD_OUT_OF_RANGE);
		}
		else
		{
			preset->sweep = arg;
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
		break;
	case AV:
		if((arg < MIN_AVE) || (arg > MAX_AVE))
		{
			uart_mini_printf(USE_UART, CMD_OUT_OF_RANGE);
		}
		else
		{
			preset->ave_num = arg;
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
		break;
	case BT:
		uart_mini_printf(USE_UART, CMD_BOOT_MODE);
		cpu_boot();
		break;
	case SF:
		if(abs(arg) > 180)
		{
			uart_mini_printf(USE_UART, CMD_OUT_OF_RANGE);
		}
		else
		{
			preset->shift = arg;
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
		break;
	case TS:
		if((arg == Termo_Int) || (arg == Termo_Ext) || (arg == Amplitude))
		{
			preset->termo_src = arg;

			preset->agc_on = 0;

			ADC_SetChan(preset->termo_src);

			if(arg == Amplitude)
				preset->agc_on = 1;

			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
		else
		{
			uart_mini_printf(USE_UART,CMD_INC_ARG);
		}
		break;
	case ED:
		if(arg > Falling_Edge)
		{
			uart_mini_printf(USE_UART,CMD_INC_ARG);
		}
		else
		{
			preset->edge = arg;

			Timer1_Configure(preset);

			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
		break;
	case MO:
		if(arg == WORK)
		{
			preset->mode = WORK;
			interface_Init(preset);
		}
		else if(arg == TA)
		{
			preset->mode = TA;
			ta_InterfaceInit();
		}
		else if(arg == UPLOAD)
		{
			ta_Upload(preset->mode);
		}
		else if(arg == DOWNLOAD)
		{
			ta_Download(preset->mode);
		}
		if(arg == PRESSURE)
		{
			preset->mode = PRESSURE;
			interface_Init(preset);
		}
		if(arg == STOP)
		{
			preset->mode = STOP;
		}
		break;
#ifdef AGC_ON
	case TH:
		agc_SetTH(arg);
		uart_mini_printf(USE_UART, CMD_SUCCESS);
		break;
	case GC:
		preset->agc_on = arg;
		uart_mini_printf(USE_UART, CMD_SUCCESS);
	break;
#endif
	case CC:
		if(ta_InitPhaseInterpPoint(arg) == SUCCESS)
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		else
			uart_mini_printf(USE_UART, CMD_INC_ARG);
		break;
#ifdef INTERFACE_TYPE_APP
	case CF:

		break;
#endif
	case SA:
		preset_Save();
		uart_mini_printf(USE_UART, CMD_SUCCESS);
		break;
	case SN:
		if((arg > TA_NUM_SENSORS) || (!ta_Valid()))
			uart_mini_printf(USE_UART, CMD_INC_ARG);
		else
		{
			preset->sens_num = arg;
			uart_mini_printf(USE_UART,"\t %s \t", ta_SensID(preset->sens_num));
			uart_mini_printf(USE_UART, CMD_SUCCESS);
		}
		break;
	case FR:
		preset->freq = arg;
		uart_mini_printf(USE_UART, CMD_SUCCESS);
		break;
	default:
		uart_mini_printf(USE_UART, CMD_UNKNOWN);
		break;
	}
}

// ������������� ��������� ���������
static int idx;

void ta_InterfaceInit()
{
	idx = 0;
	UART_Configure(UART_TA_BAUD);
}

// �������� ������ � ������ ���������
int ta_Send()
{
	static float Period, termo, T;
	static uint16_t dig1, dig2;

	if(idx++ == 0)
	{
#ifdef INTERFACE_TYPE_APP
		Period = (pack.T * Tq_ms) / preset->ave_num;
		termo = pack.termo/4096.0f * 0.0003f;
		pack.termo = 0;
		UART_PutChar(USE_UART, TA_MARK);
#endif
#ifdef INTERFACE_TYPE_MATLAB
		T = (float)preset->pack->T;
		Period = (T * Tq_ms) / (float)preset->ave_num;
		pack.T = 0;
		termo = (float)((3 * pack.termo) >> AGC_D)/4096.0f * 0.0001f;
#ifndef AGC_RECU
		pack.termo = 0;
#endif
		UART_PutChar(USE_UART, TA_MARK);
#endif
		return idx;
	}
	else if(idx <= TA_WORD_LENGTH + 1)
	{
		Period = 10.0f * Period - 10.0f * dig1;
		dig1 = Period;
		UART_PutChar(USE_UART, dig1);
		return idx;
	}
	else if(idx <= (TA_WORD_LENGTH << 1) + 1)
	{
		termo = 10.0f * termo - 10.0f * dig2;
		dig2 = termo;
		UART_PutChar(USE_UART, dig2);
		return idx;
	}
	else
	{
		idx = 0;
		dig1 = 0;
		dig2 = 0;
	}

	return idx;
}

void cmdUpd()
{
	static int i;
	uint8_t ch;

	ch = UART_GetChar(USE_UART);


	if((ch != '\r') && (i < (UART_DATA_BUF_LEN - 1u)))
	{
		uart_cmd.data[i++] = ch;
		UART_PutChar(USE_UART, ch);
	}
	else
	{
		uart_cmd.data[i] = 0u;
		dataRcv();
		i = 0;
	}
}

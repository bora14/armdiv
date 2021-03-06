/*
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
#ifdef AGC_ON
#include "agc.h"
#endif

#if defined(INTERFACE_TYPE_MATLAB)
static pack_t pack = {SOT, 0, 0, EOT};
#endif
#if defined(INTERFACE_TYPE_APP)
static pack_t pack = {SOT, 0, 0, 0, 0.0f, 0, 0};
#endif

static uart_cmd_t * _uart_cmd;
static Preset_t * preset;

void interface_Init(Preset_t * _preset)
{
	preset = _preset;

	preset->pack = &pack;

	UART_Configure(UART_WORK_BAUD);

	_uart_cmd = uart_Cmd();
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
		UART_PutChar(USE_UART, *p++);
	}
#ifndef AGC_RECU
	pack.termo = 0;
#endif

	pack.T = 0;
#endif
}

pack_t * get_Pack()
{
	return &pack;
}

void dataRcv()
{
	int32_t arg;

	arg = atol((char *)&_uart_cmd->data[UART_MARK_LEN]);

	switch(_uart_cmd->Mrk)
	{
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
		T = (float)preset->T[(preset->t-1) & 0x01];
		Period = (T * Tq_ms) / (float)preset->ave_num;
		pack.T = 0;
		termo = (float)(pack.termo >> 10)/4096.0f * 0.0003f;
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

/**
 * \file main.c
 *
 *  Created on: 29.11.2015
 *      Author: Ivan
 */

#include "stdlib.h"
#include "board.h"
#include "dpll.h"
#include "uart.h"
#include "timer.h"
#include "interface.h"
#include "adc.h"
#include "termo.h"
#include "port.h"
#include "exceptions.h"
#include "agc.h"
#include "ta.h"
#include "memo.h"
#include "dma.h"
#ifdef POWER_SAVE_MODE_ON
#include "MDR32F9Qx_power.h"
#include "MDR32F9Qx_bkp.h"
#include "MDR32F9Qx_iwdg.h"
#endif
#ifdef AGC_ON
#include "agc.h"
#endif
#include <string.h>

static int flgDataTr = 0;
/**
 * Переменная для хранения настроек СЦВД.
 */
static Preset_t preset;

static int preset_Init();

int main()
{
	__disable_irq();

	/* Инициализация контроллера EEPROM */
	memo_Configure(MEMO_TA_BANK_SEL);
	/* Инициализация структуры PRESET_T */
	preset_Init();
	/* Инициализация GPIO */
	Port_Configure();
	// Инициализация контроллера прерываний
	Intr_Init(&preset);
	/* Инициализация UART */
	interface_Init(&preset);
#ifdef DMA
	dma_Init(DMA_UART_TX_CHANNEL, (uint32_t) preset.pack, (uint32_t) &USE_UART->DR);
#endif

	uart_mini_printf(USE_UART, "\n\r Timer1 Configure......");
	Timer1_Configure(&preset);
	uart_mini_printf(USE_UART, "OK!\n\r");

	uart_mini_printf(USE_UART, "\n\r Timer3 Configure......");
	Timer3_Configure(&preset);
	uart_mini_printf(USE_UART, "OK!\n\r");

	uart_mini_printf(USE_UART, "\n\r ADC Configure......");
	ADC_Configure(&preset);
	uart_mini_printf(USE_UART, "OK!\n\r");

	termo_Init(&preset);

	uart_mini_printf(USE_UART, "\n\r AGC Configure......");
	agc_Init(&preset);
	uart_mini_printf(USE_UART, "OK!\n\r");

	uart_mini_printf(USE_UART, "\n\r DPLL Init......");
	dpll_Init(&preset);
	uart_mini_printf(USE_UART, "OK!\n\r");

	uart_mini_printf(USE_UART, "\n\r TA Table Init......");
	ta_Init(&preset);
	uart_mini_printf(USE_UART, "OK!\n\r");

	__enable_irq();

	uart_mini_printf(USE_UART, "\r\n GO-GO-GO! \n\r");

	static ave_cnt;

	for(;;)
	{
		if(dpll_UpdFlg())
		{
			dpll_ClearUpdFlg();

			dpll_Update();

			preset.T[0] += MDR_TIMER1->ARR;

			agc_Termo();

#ifndef AGC_RECU
			if(preset.termo_src == Amplitude)
				preset.pack->termo = preset.amp;
			else
				preset.pack->termo += termo_Val();
#endif
			if(++ave_cnt >= preset.ave_num)
			{
#ifdef AGC_RECU
				if(preset.termo_src == Amplitude)
					preset.pack->termo = preset.amp;
				else
					preset.pack->termo = preset.termo;
#endif
				preset.T[0] += preset.ave_num;
				preset.pack->T = preset.T[0];

				if(preset.mode == PRESSURE)
				{
					preset.pack->P = Calc_Pressures((MAX_AVE * (uint64_t)preset.T[0])/preset.ave_num,
							SHR(3 * 10000 * (uint64_t)preset.amp, AGC_D + ADC_RESOL),
							preset.sens_num);
				}

				preset.T[0] = 0;
				ave_cnt = 0;

				preset.t++;

				if(preset.mode == PRESSURE)
					setFlgDataTr();
			}
		}

		if(getFlgDataTr())
		{
			if((preset.mode == WORK) || (preset.mode == PRESSURE))
			{
				clearFlgDataTr();
				dataTr();
			}
			else if(preset.mode == TA)
			{
				if(ta_Send() == 0)
					clearFlgDataTr();
			}
			else if(preset.mode == STOP)
			{
				clearFlgDataTr();
			}
		}
		if(uart_GetRxComplete())
		{
			uart_ClearRxComplete();
			cmdUpd();
		}
		if(preset.agc_start)
		{
			preset.agc_start = 0;
			agc_Amp();

			dpll_Fail();
		}

#ifdef POWER_SAVE_MODE_ON
		POWER_EnterSTOPMode(ENABLE, POWER_STOPentry_WFE);
#endif
	}
	return 0;
}

void setFlgDataTr()
{
	flgDataTr = 1;
}

int getFlgDataTr()
{
	return flgDataTr;
}

void clearFlgDataTr()
{
	flgDataTr = 0;
}

void LED_On(uint32_t led)
{
	LED_PORT->RXTX |= led;
}

void LED_Off(uint32_t led)
{
	LED_PORT->RXTX &= ~led;
}

void LED_Blink(uint32_t led)
{
	LED_PORT->RXTX ^= led;
}

void AMP_Ctrl(int16_t amp)
{
	preset.att = amp;
}

/**
 * Перевод МК в режим загрузчика UART2.
 */
void cpu_boot()
{
	BKP_DeInit();

	MDR_BKP->REG_0E = 6 << 12; // Reset to BOOT UART2 Mode

	IWDG_WriteAccessEnable();
	IWDG_SetReload(1);
	IWDG_Enable();
}

/**
 * \brief Инициализация СЦВД
 *
 * Инициализация переменной preset
 * в которой хранятся настройки СЦВД.
 * \return - 0 - если в ПЗУ не записан ключ;
 * 			 1 - в противном случае.
 */
int preset_Init()
{
	uint8_t key = 0, ret = 0;

	key = memo_ReadByte(MEMO_CONF_BASE_ADR + MEMO_CONF_KEY_REG, MEMO_CONF_BANK_SEL);

	if(key == MEMO_CONF_KEY)
	{
		ret = 1;
		memo_Read(MEMO_CONF_BASE_ADR + MEMO_CONF_PRE_REG, MEMO_CONF_BANK_SEL, (uint8_t *)&preset, MEMO_CONF_PRE_LEN);
	}
	else
	{
		preset.sweep = MIN_SWEEP;
		preset.ave_num = AVE_NUM;
		preset.Tmax = DPLL_T_MAX;
		preset.Tmin = DPLL_T_MIN;
		preset.filt_order = LOOP_FILTER_ORDER;
		preset.edge = Rising_Edge;
		preset.shift = 0;
		preset.att0 = 0;
		preset.att = preset.att0;
	#ifdef AGC_ON
		preset.agc_th = AGC_TH;
	#endif
		preset.agc_on = 1;
		preset.mode = WORK;
		preset.search_th = 100;
		preset.search_len = AMP_SEARCH_POINTS_NUM;
		preset.search_fl = 1000;
		preset.termo_src = Amplitude;
		preset.type_md = MD9;
	}
	preset.sot = 0x10203040;
	preset.es = 1;
	preset.agc_start = 0;
	preset.t = 0;
	preset.amp = 0;
	preset.T[0] = 0;
	preset.T[1] = 0;

	return ret;
}

/**
 * Сохранение настроек СЦВД в ПЗУ
 */
void preset_Save()
{
	/*
	 * Стирание банка памяти в котором хранятся
	 * настройки СЦВД
	 */
	memo_Erase(MEMO_CONF_BASE_ADR, MEMO_CONF_BANK_SEL);
	/*
	 * Запись новых настроек СЦВД
	 */
	memo_Write(MEMO_CONF_BASE_ADR + MEMO_CONF_PRE_REG, MEMO_CONF_BANK_SEL,
			(uint8_t *)&preset, MEMO_CONF_PRE_LEN);
	/*
	 * Записть ключа
	 */
	memo_WriteByte(MEMO_CONF_BASE_ADR + MEMO_CONF_KEY_REG, MEMO_CONF_BANK_SEL, MEMO_CONF_KEY);
}

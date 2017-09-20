/**
 * \file
 *
 * interface.h
 *
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "board.h"
#include "uart.h"

#ifdef INTERFACE_TYPE_MATLAB
#	define SOT					(0x01020304) // Start of transmission
#	define EOT 					(0x00) // End of transmission
#endif
#ifdef INTERFACE_TYPE_APP
#	define SOT					(0xFB14A75D)
#	define PRESET_MARK			(0xD57A41BF)
#endif

#define MESSAGE_LEN 		(sizeof(pack_t))
/* Протокол тарировки */
#define TA_MARK			(0xAA)
#define TA_WORD_LENGTH	(7u) // длинна слова для тарировки
#define TA_PACK_LENGTH	((TA_WORD_LENGTH << 1) + 1)

#define CMD_SUCCESS				"\t OK!\r\n"
#define CMD_UNKNOWN 			"\t CMD unknown\r\n"
#define CMD_BOOT_MODE			"\t CPU Boot Mode...\r\n"
#define CMD_OUT_OF_RANGE		"\t out of bound\r\n"
#define CMD_INC_ARG				"\t incorrect argument\r\n"

/**
 * Список управляющих комманд
 */
typedef enum
{
#ifdef NUMBER_VERSION
	LS = 'SL',
#endif
#ifdef AGC_ON
	TH = 'HT',	///< Порог срабатывания АРУ
	GC = 'CG',	///< Включение/выключение АРУ
#endif
#ifdef INTERFACE_TYPE_APP
	CF = 'FC',
#endif
	UP = 'PU',  ///< Задание верхней границы поиска, в Гц
	DN = 'ND',	///< Задание нижней границы поиска, в Гц
	AM = 'MA',	///< Управление амплитудой (1 + cos(pi*AM/180)), в град
	SW = 'WS',	///< Задание скорости сканирования
	AV = 'VA',	///< Длина выборки для усреднения периода
	BT = 'TB',	///< Перевод МК в режим прошивки
	SF = 'FS',	///< Установка сдвига фаз между входом и выходом, в град
	TS = 'ST',	///< Выбор канала АЦП (ADC_Channel)
	ED = 'DE',	///< Выбор фронта выходного сигнала
	MO = 'OM',	///< Выбор режима работы МК (OPMODE_T)
	CC = 'CC',
	SA = 'AS',	///< Сохранение настроек СЦВД
	SN = 'NS',	///< Выбор градуированного датчика
	FR = 'RF'  ///< Измеренная частота работы МК
}Cmd_t;

typedef enum
{
	TA_DISABLE = 0,
	TA_ENABLE = 1
}enum_ta_t;

typedef enum
{
	INT_WORK,
	INT_TA
}int_mode_t;


/*************************/

void interface_Init(Preset_t * preset);

void dataTr();

pack_t * get_Pack();

void dataRcv();

void ta_InterfaceInit(); // инициализация протокола тарировки

int ta_Send(); // передача данных в режиме тарировки

int_mode_t get_Mode();

void cmdUpd();

#endif /* INTERFACE_H_ */

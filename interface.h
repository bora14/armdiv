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
#	define SOT					(0xaa55) // Start of transmission
#	define EOT 					(0x00) // End of transmission
#endif
#ifdef INTERFACE_TYPE_APP
#	define SOT					(0xFB14A75D)
#	define PRESET_MARK			(0xD57A41BF)
#endif

#define MESSAGE_LEN 		(sizeof(pack_t))
/* �������� ��������� */
#define TA_MARK			(0xAA)
#define TA_WORD_LENGTH	(7u) // ������ ����� ��� ���������
#define TA_PACK_LENGTH	((TA_WORD_LENGTH << 1) + 1)

#define CMD_SUCCESS				"\t OK!\r\n"
#define CMD_UNKNOWN 			"\t CMD unknown\r\n"
#define CMD_BOOT_MODE			"\t CPU Boot Mode...\r\n"
#define CMD_OUT_OF_RANGE		"\t out of bound\r\n"
#define CMD_INC_ARG				"\t incorrect argument\r\n"

/**
 * ������ ����������� �������
 */
typedef enum
{
#ifdef AGC_ON
	TH = 'HT',	///< ����� ������������ ���
	GC = 'CG',	///< ���������/���������� ���
#endif
#ifdef INTERFACE_TYPE_APP
	CF = 'FC',
#endif
	UP = 'PU',  ///< ������� ������� ������� ������, � ��
	DN = 'ND',	///< ������� ������ ������� ������, � ��
	AM = 'MA',	///< ���������� ���������� (1 + cos(pi*AM/180)), � ����
	SW = 'WS',	///< ������� �������� ������������
	AV = 'VA',	///< ����� ������� ��� ���������� �������
	BT = 'TB',	///< ������� �� � ����� ��������
	SF = 'FS',	///< ��������� ������ ��� ����� ������ � �������, � ����
	TS = 'ST',	///< ����� ������ ��� (ADC_Channel)
	ED = 'DE',	///< ����� ������ ��������� �������
	MO = 'OM',	///< ����� ������ ������ �� (OPMODE_T)
	CC = 'CC',
	SA = 'AS'
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

void ta_InterfaceInit(); // ������������� ��������� ���������

int ta_Send(); // �������� ������ � ������ ���������

int_mode_t get_Mode();

#endif /* INTERFACE_H_ */

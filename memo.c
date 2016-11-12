/*
 * \file memo.c
 *
 *  Created on: 26.01.2016
 *      Author: �������
 */
#include "memo.h"

/**
 * \brief ������� ���������������� ���.
 *
 * ���������� ������� ���.
 *
 * param Bank - ���� ������ (����������� �� �������������).
 */
void memo_Configure(uint32_t Bank)
{
	__disable_irq();

	EEPROM_SetLatency(EEPROM_Latency_4);

	__enable_irq();
}

/**
 * ������� ������ ������ ����� ������ �� ���.
 *
 * param addr - ����� ����� ������.
 * param Bank - ���� ������ ��� ��������� (����������� �� �������������).
 */
uint8_t memo_ReadByte(uint32_t addr, uint32_t Bank)
{
	uint8_t byte;

	__disable_irq();

	byte = EEPROM_ReadByte(addr, Bank);

	__enable_irq();

	return byte;
}

/**
 * ������� ������ ������ ����� � ���.
 *
 * param addr - ����� ���� ����� ������� ���� ����.
 * param Bank - ���� ������ ��� ��������� (����������� �� �������������).
 * param byte - �������� ����� ��� ������.
 */
void memo_WriteByte(uint32_t addr, uint32_t Bank, const uint8_t byte)
{
	__disable_irq();

	EEPROM_ProgramByte(addr, Bank, byte);

	__enable_irq();
}

/**
 * ������� ������ ������� ������ � ���.
 *
 * param addr - ����� ������ ������� ������ � ���, ������� ���������� �������������������.
 * param Bank - ���� ������ ��� ��������� (����������� �� �������������).
 * param *data - ��������� �� ������ ������ ��� ������ � ���.
 * param len - ����� ������� � ������.
 */
void memo_Write(uint32_t addr, uint32_t Bank, uint8_t * data, uint32_t len)
{
	uint32_t i;

	__disable_irq();

	for(i = 0; i < len;  i++)
	{
		EEPROM_ProgramByte(addr, Bank, *data);
		addr++;
		data++;
	}

	__enable_irq();
}

/**
 * ������� ������ ������� ������ �� ���.
 *
 * param addr - ����� ������ ������� ������, ���������� � ���.
 * param Bank - ���� ������ ��� ��������� (����������� �� �������������).
 * param *data - ��������� �� ������, ������� ����� ������������������ ������� �� ���.
 * param len - ����� �������  ������.
 */
void memo_Read(uint32_t addr, uint32_t Bank, uint8_t * data, uint32_t len)
{
	uint32_t i;

	__disable_irq();

	for(i = 0; i < len;  i++)
	{
		*data = EEPROM_ReadByte(addr, Bank);
		addr++;
		data++;
	}

	__enable_irq();
}

/**
 * ������� �������� ������� ���.
 *
 * param addr - ����� ������ ������� ���, ������� ���������� �������.
 * param Bank - ���� ������ ��� ��������� (����������� �� �������������).
 */
void memo_Erase(uint32_t addr, uint32_t Bank)
{
	__disable_irq();

	EEPROM_ErasePage(addr, Bank);

	__enable_irq();
}

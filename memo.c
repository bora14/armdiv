/*
 * \file memo.c
 *
 *  Created on: 26.01.2016
 *      Author: Сламапа
 */
#include "memo.h"

/**
 * \brief Функция конфигурирования ПЗУ.
 *
 * Производит очистку ПЗУ.
 *
 * param Bank - банк памяти (указывается по необходимости).
 */
void memo_Configure(uint32_t Bank)
{
	__disable_irq();

	EEPROM_SetLatency(EEPROM_Latency_4);

	__enable_irq();
}

/**
 * Функция чтения одного байта данных из ПЗУ.
 *
 * param addr - адрес байта данных.
 * param Bank - банк памяти для обращения (указывается по необходимости).
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
 * Функция записи одного байта в ПЗУ.
 *
 * param addr - адрес куда будет записан один байт.
 * param Bank - банк памяти для обращения (указывается по необходимости).
 * param byte - значение байта для записи.
 */
void memo_WriteByte(uint32_t addr, uint32_t Bank, const uint8_t byte)
{
	__disable_irq();

	EEPROM_ProgramByte(addr, Bank, byte);

	__enable_irq();
}

/**
 * Функция записи массива данных в ПЗУ.
 *
 * param addr - адрес начала массива данных в ПЗУ, который необходимо проинициализировать.
 * param Bank - банк памяти для обращения (указывается по необходимости).
 * param *data - указатель на массив данных для записи в ПЗУ.
 * param len - длина массива в байтах.
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
 * Функция чтения массива данных из ПЗУ.
 *
 * param addr - адрес начала массива данных, хранящихся в ПЗУ.
 * param Bank - банк памяти для обращения (указывается по необходимости).
 * param *data - указатель на массив, который будет проинициализирован данными из ПЗУ.
 * param len - длина массива  байтах.
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
 * Функция стирания области ПЗУ.
 *
 * param addr - адрес начала области ПЗУ, которую необходимо стиреть.
 * param Bank - банк памяти для обращения (указывается по необходимости).
 */
void memo_Erase(uint32_t addr, uint32_t Bank)
{
	__disable_irq();

	EEPROM_ErasePage(addr, Bank);

	__enable_irq();
}

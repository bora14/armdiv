/*
 * memo.h
 *
 *  Created on: 25.01.2016
 *      Author: Сламапа
 */

#ifndef MEMO_H_
#define MEMO_H_

#include "board.h"
#include "MDR32F9Qx_eeprom.h"

#define MEMO_PAGE_SZ		(4096)
#define MEMO_PAGE_NUM		(32)

#define MEMO_DATA_KEY		(0x55)
#define MEMO_CC_KEY			(0x55)

#define MEMO_TA_BANK_SEL	EEPROM_Info_Bank_Select
#define MEMO_CC_KEY_REG		(0x00)
#define MEMO_CC1_REG		(0x01)
#define MEMO_CC2_REG		(0x02)
#define MEMO_DATA_KEY_REG	(0x03)
#define MEMO_DATA_ADDR		(0x04)

#define MEMO_CONF_BANK_SEL	EEPROM_Main_Bank_Select
#define MEMO_CONF_KEY		(0x55) ///< Ключ наличия в ПЗУ первоначальных настроек СЦВД
#define MEMO_CONF_PRE_LEN	(sizeof(Preset_t))
#define MEMO_CONF_BASE_ADR	((MEMO_PAGE_NUM - 1) * MEMO_PAGE_SZ)
#define MEMO_CONF_KEY_REG	(0x00)
#define MEMO_CONF_PRE_REG	MEMO_CONF_PRE_LEN

void memo_Configure(uint32_t Bank);

uint8_t memo_ReadByte(uint32_t addr, uint32_t Bank);

void memo_WriteByte(uint32_t addr, uint32_t Bank, const uint8_t byte);

void memo_Read(uint32_t addr, uint32_t Bank, uint8_t * data, uint32_t len);

void memo_Write(uint32_t addr, uint32_t Bank, uint8_t * data, uint32_t len);

void memo_Erase(uint32_t addr, uint32_t Bank);

#endif /* MEMO_H_ */

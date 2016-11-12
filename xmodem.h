/*
 * xmodem.h
 *
 *  Created on: 13.09.2011
 *      Author: iv14
 */

#ifndef XMODEM_H_
#define XMODEM_H_

#include "board.h"

#define XMODEM_SOH					(0x01u)		///< ������ ������ ���������
#define XMODEM_EOT					(0x04u)		///< ������ ����� ������
#define XMODEM_ACK					(0x06u)		///< ������ �������������� �������������
#define XMODEM_NAK					(0x15u)		///< ������ �������������� �������������
#define XMODEM_ETB					(0x17u)		///< ������ ��������� ��������
#define XMODEM_CAN					(0x18u)		///< ������ ������ ������
#define XMODEM_C					(0x43u)		///< ������ ������� �� ��������
#define XMODEM_LEN_BLOCK			(128u)		///< ����� ����� ������
#define XMODEM_TIMEOUT_STATE_Yes	(1u)
#define XMODEM_TIMEOUT_STATE_No		(0u)
#define XMODEM_WAIT_NUM				(20u)

void xmoden_init();

uint8_t xmodem_trans(const uint8_t *data, const uint32_t len);

uint8_t xmodem_recv(uint8_t *data, uint32_t len);

void xmodem_restart();

void xmodem_set_timeout();

uint8_t xmodem_get_timeout();

void xmodem_clear_timeout();

uint8_t xmodem_wait_ch();

uint16_t crc_xmodem_update(uint16_t * crc, uint8_t * ch);

#endif /* XMODEM_H_ */

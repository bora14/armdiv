/**
 * \file termo.h
 * \brief
 *
 * ���� �������� ���������� ������� ���
 * ������ ��������� ������� �����������.
 */

#ifndef TERMO_H_
#define TERMO_H_

#include "board.h"

/**
 * ������������� ���������� ��� ������ � ��������� �����������.
 *
 */
void termo_Init(Preset_t * preset);

/**
 * ��������� ���������� �� ������ ������� �����������.
 */
uint16_t termo_Val();

/**
 * ����� ������������� ������� �����������.
 */
void termo_SetChan(uint8_t chan);

#endif /* TERMO_H_ */

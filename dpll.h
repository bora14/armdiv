/** \file
 * dpll.h
 *
 *  Created on: 29.11.2015
 *      Author: Ivan
 */

#ifndef DPLL_H_
#define DPLL_H_

#include "board.h"

/**
 * ���������� ��������� �������� ������������ �������� ��������� �������
 */
#define DPLL_GAIN_ITEMS	7

/**
 * ������������� ������� ����
 */
int32_t dpll_Init(Preset_t * preset_);

/**
 * ��������� �� ��������� � ������� ��������� ��������� ����
 */
dpll_t * dpll_Get();

/**
 * \brief ���������� ��������� �������� ��������� �������.
 *
 * ������� ����������� � ����������� ������ ��� ��������� ������
 * ���� �������.
 */
int32_t dpll_Filt(dpll_t * arg);
/**
 * ����� ��������� �������
 */
void dpll_ClearFilt();
/**
 * ���� ���������� ������ �������� ���������
 */
uint8_t dpll_UpdFlg();
/**
 * ��������� �������� �������� ���������
 */
void dpll_ClearAcc();
/**
 * ��������� ��������� �������� ��������� �������
 */
void dpll_ClearPhi();
/**
 * ��������� ����� ���������� �������� ���������
 */
void dpll_SetUpdFlg();
/**
 * ����� ����� ���������� �������� ���������
 */
void dpll_ClearUpdFlg();
/**
 * ���������� ��������� ����
 */
void dpll_Update();
/**
 * �������� ������.
 */
float dpll_LoopFilter(float x, float *z, float *a, uint8_t order);

#endif /* DPLL_H_ */

/*
 * \file ta.h
 *
 * Файл содержит объвления функций, необходимых для работы с
 * тарировочными данными.
 */

#ifndef TA_H_
#define TA_H_

#include "board.h"

#define TA_NUM_SENSORS 		(6u)       /* Количество датчиков давления */
#define TA_BLOCK_ID_LEN		(8u)
#define TA_PRESS_NUM_STEP	(14u)
#define TA_TEMP_NUM_STEP	(11u)
#define TA_DDG_TN_LEN		TA_NUM_SENSORS
#define TA_DDG_T0_LEN		TA_NUM_SENSORS
#define TA_DDG_TS_LEN		TA_NUM_SENSORS
#define TA_DDG_TK_LEN		TA_NUM_SENSORS
#define TA_DDG_UN_LEN		TA_NUM_SENSORS
#define TA_DDG_U0_LEN		TA_NUM_SENSORS
#define TA_DDG_US_LEN		TA_NUM_SENSORS
#define TA_DDG_UK_LEN		TA_NUM_SENSORS
#define TA_DDG_OS_LEN		TA_NUM_SENSORS
#define TA_DDG_PA_LEN		(TA_NUM_SENSORS * TA_PRESS_NUM_STEP * TA_TEMP_NUM_STEP)

#define TA_DATA_LEN			(TA_BLOCK_ID_LEN + \
							TA_DDG_TN_LEN + TA_DDG_T0_LEN + TA_DDG_TS_LEN + TA_DDG_TK_LEN + \
							TA_DDG_UN_LEN + TA_DDG_U0_LEN + TA_DDG_US_LEN + TA_DDG_UK_LEN + \
							TA_DDG_OS_LEN + TA_DDG_PA_LEN)
/**
 * \defgroup TA_VALID	Флаг наличия тарировочных данных
 * @{
 */
#define TA_PHASE1_VALID_MSK		(0x01) /** Флаг наличия 1-ой точки для интерполяции фазы */
#define TA_PHASE2_VALID_MSK		(0x02) /** Флаг наличия 2-ой точки для интерполяции фазы */
#define TA_TABLE_VALID_MSK		(0x04) /** Флаг наличия тарировочной таблицы для интерполяции давления */
/** @} */

/*-- Структура для представления входных и выходных данных алгоритма ---------*/
typedef struct{
	int32_t Value;  /* Значение параметра */
	int32_t Valid;  /* Исправность параметра (1 - исправно)*/
} Normal_Data;

/*-- Переменные четырехточечной сплайн-интерполяции --------------------------*/
typedef struct{
	int32_t j; /* Номер крайнего левого узла                                        */
	int32_t z; /* Расстояние до базового узла (обычно второго) : 2^14 = 16384       */
} _INT;

typedef struct
{
	char BlockID[TA_BLOCK_ID_LEN];
	uint16_t DDG_TN[TA_DDG_TN_LEN];            /* nodes number for T-axis   */
	int32_t DDG_T0[TA_DDG_T0_LEN];             /* minimums of T            */
	int32_t DDG_TS[TA_DDG_TS_LEN];             /* T multiplying coefficients*/
	uint16_t DDG_TK[TA_DDG_TK_LEN];            /* T scale factors          */
	uint16_t DDG_UN[TA_DDG_UN_LEN];            /* nodes number for U-axis  */
	int32_t DDG_U0[TA_DDG_U0_LEN];             /* minimums of U            */
	int32_t DDG_US[TA_DDG_US_LEN];             /* U multiplying coefficients*/
	uint16_t DDG_UK[TA_DDG_UK_LEN];            /* U scale factors          */
	uint16_t DDG_OS[TA_DDG_OS_LEN];            /* Offsets in interp table  */
	int32_t DDG_PA[TA_DDG_PA_LEN];             /* interpolation tables    */
} DDG_Koef __attribute__ ((aligned (8)));

void ta_Init(Preset_t * preset_);
int32_t Calc_Pressures(int32_t T, int32_t U, uint16_t K);
int32_t DDG(int32_t * T, int32_t * U, uint16_t * K, DDG_Koef *Tbl);
_INT indicINT(int32_t x, int32_t n);
int32_t fintINT(const int32_t * f, int32_t x);

uint8_t ta_InitPhaseInterpPoint(const int8_t point);

int32_t ta_PhaseInterp(const int32_t * period, const int32_t ave);

int8_t ta_Upload(opmode_t mode);

int8_t ta_Download(opmode_t mode);

void ta_WriteTable();

void ta_InitTable();

#endif /* TA_H_ */

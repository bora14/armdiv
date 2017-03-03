/** \file
 * board.h
 *
 *  Created on: 29.11.2015
 *      Author: Ivan
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "core_cm3.h"
#include "MDR32F9Qx_config.h"

#define MCU_UCC		3	///< Напряжение питания МК, В
//#define DMA	///< Включение ПДП режима UART
#define UART_FIFO_TR
#define XMODEM
#define POWER_SAVE_MODE_ON ///< Включение режима пониженного энергопотребления
/**
 * Тип схемы для управления амплитудой входного сигнала.
 * 1 - упралвение через ШИМ;
 * 2 - управление по средствам сдвига фаз.
 */
#define SCH_TYPE	(2)

//#define AGC_ON			///< включение АРУ
#define AGC_RECU		///< Рекурсивная оценка значения АЦП

//#define FLL_ASSISTED	///< петля ФАПЧ с частотной поддержкой

#define INTERFACE_TYPE_MATLAB
//#define INTERFACE_TYPE_APP

#if (!defined(INTERFACE_TYPE_MATLAB)) && (!defined(INTERFACE_TYPE_APP))
#	error "undefined INTERFACE_TYPE. Must be INTERFACE_TYPE_APP or INTERFACE_TYPE_MATLAB"
#endif


/**
 *  Приоритеты исключений
 */
#define TIMER1_EXP_PRIOR	(0)
#define TIMER2_EXP_PRIOR	(1)
#define TIMER3_EXP_PRIOR	(2)
#define DMA_EXP_PRIOR		(3)
#define UART2_EXP_PRIOR 	(4)

#define USE_UART 	MDR_UART2
#define UART_WORK_BAUD	(115200u)
#define UART_TA_BAUD	(19200u)
#if SCH_TYPE == 1
#define AMP_Timer	MDR_TIMER3
#endif
#if SCH_TYPE == 2
#define AMP_Timer	MDR_TIMER1
#endif
#define DPLL_TIMER				MDR_TIMER1
#define DPLL_TIMER_IRQ			Timer1_IRQn
#define DPLL_TIMER_Prescaler	TIMER1_Prescaler

#define TIMER1_Prescaler (0u)
#define TIMER2_Prescaler (1000u)
#define TIMER3_Prescaler (0u)

#if SCH_TYPE == 1
#define LED_PORT 	(MDR_PORTB)
#define LED_PINS	(PORT_Pin_0)
#define LED1		(PORT_Pin_0)
#endif

#if SCH_TYPE == 2
#define LED_PORT 	(MDR_PORTB)
#define LED_PINS	(PORT_Pin_1)
#define LED1		(PORT_Pin_1)
#define LED2		(PORT_Pin_3)
#endif

#define DPLL_VER 3

#define MAX_SWEEP 	(30u)
#define MIN_SWEEP 	(1u)
#define MAX_AVE 	(1000)
#define MIN_AVE 	(100)
#define AVE_NUM 	(200)
#define DPLL_F_MAX 	(20000u) 	///< Верхняя граница диапазона частот, Гц
#define DPLL_F_MIN 	(5000u)		///< Нижняя граница диапазона частот, Гц
#define Tq 			(1.0f / ((float)CPU_MCK)) // квант времени
#define Tq_ms		(Tq * 1000.0f)
#define Tq_us		(Tq * 1000000.0f)

#if SCH_TYPE == 1
#define PWR_CTRL_PERIOD  (600u)
#define PWR_CTRL_PERIOD_MAX (2 * PWR_CTRL_PERIOD - 5u)
#define PWR_CTRL_PERIOD_MIN (5u)
#elif SCH_TYPE == 2

#define DPLL_T_MAX 		(CPU_MCK/((DPLL_TIMER_Prescaler + 1u) * DPLL_F_MAX))
#define DPLL_T_MIN 		(CPU_MCK/((DPLL_TIMER_Prescaler + 1u) * DPLL_F_MIN))

#define DPLL_T(Freq)	(CPU_MCK/((DPLL_TIMER_Prescaler + 1u) * Freq))
#ifdef AGC_ON
#	define AGC_TH	(1700)
#endif
#endif

#define AGC_FREQ	(DPLL_T_MAX >> 1)

/**
 * \defgroup LOOP_FILTER_ORDER Порядок петлевого фильтра
 */
#define LOOP_FILTER_ORDER	(3u)

#define sign(x) ((x) > 0) ? 1 : (((x) < 0) ? -1 : 0)

#define SUCCESS		(1u)
#define FAILURE		(0u)

typedef enum
{
	Rising_Edge,
	Falling_Edge
}edge_t;

#ifdef FLL_ASSISTED
/**
 * Переменная типа dAc_t необходима для хранения
 * последовательности 4 результатов работы фазового детектора.
 * Данные значения используются для частотной
 * поддержки петли ФАПЧ.
 */
typedef union
{
	uint64_t dAc;
	int16_t dAc_arr[4];
}dAc_t;
#endif

/**
 * Структура для хранения параметров системы ФАПЧ.
 */
typedef struct
{
	int32_t T0; 	///< Текущее значение основания счета таймера
	float T; 		///< Текущая оценка периода входного сигнала
	float Phi; 		///< Выходное значение петлевого фильтра
#ifndef FLL_ASSISTED
	int32_t * dAc; 	///< Указатель на выходные значения фазового детектора
#else
	dAc_t dAc;	///< Указатель на выходные значения фазового детектора
#endif
	float Acc; 	///< Указатель на входные значения петлевого фильтра
	int32_t cnt;	///< Номер периода для которого получено значение фазового детектора
	uint8_t updflg; ///< Флаг готовности данных фазового детектора
	int8_t * intr;	///< Указатель на признак наличия сигнала на входе
	float dPhi;		///< Выходное значение интегратора в петлевом фильтре
	int8_t ld;		///< Флаг захвата частоты ФАПЧ
	int32_t shift;	///< Сдвиг фаз между входом и выходом, в тактах
	int32_t phase;	///< Сдвиг фаз между входом и выходом, в град
	int32_t phase1;	///< Сдвиг фаз в 1-ой точке интерполяционной прямой, в град
	int32_t period1;///< Период резонансной частоты в 1-ой точке интерполяционной прямой, в тактах
	int32_t phase2;	///< Сдвиг фаз в 2-ой точке интерполяционной прямой, в град
	int32_t period2;///< Период резонансной частоты во 2-ой точке интерполяционной прямой, в тактах
	int8_t interp_valid_ta;///< Флаг наличия тарировочных коэффициентов
}dpll_t;


/**
 * \defgroup INTERFACE_TYPE Протокол передачи
 *
 * Описание структуры для хранения измерений
 *
 * @{
 */
#ifdef INTERFACE_TYPE_APP

#	define LD_MSK				(1 << 12u)

typedef struct
{
	const uint32_t mark;
	uint8_t t;
	int32_t T; 		///< Current Period Natural Freq
	int16_t phase;
	float P;
	uint32_t amp;
	uint16_t termo;
}__attribute__((packed)) pack_t;
#endif

#ifdef INTERFACE_TYPE_MATLAB
typedef struct
{
	uint16_t sot;
	uint32_t T;
	uint32_t P;
	uint32_t termo;
	uint8_t eot;
}__attribute__((packed)) pack_t;
#endif

/** @} */

/**
 * \defgroup OPMODE_T Режим работы
 * Определяет текущий режим работы СЦВД.
 * @{
 */
typedef enum
{
	WORK = 0, 		///< Нормальная работа
	TA = 1,			///< Тарировка
	UPLOAD = 2,		///< Загрузка данных
	DOWNLOAD = 3,	///< Выгрузка данных
	PRESSURE = 4	///< Измерение давления
}opmode_t;
/** @} */

/**
 * \defgroup PRESET_T Настройки
 *
 * \brief Структура для хранения настроек СЦВД.
 *
 * __attribute__((packed)) (выравнивание по 1 байту) необходим чтобы данную структуру можно было
 * принять любым компилятором.
 *
 * @{
 */
typedef struct
{
	int32_t sweep; 		///< Sweep speed
	int32_t ave_num; 	///< average number
	int32_t Tmax; 		///< Max Period Natural Freq
	int32_t Tmin; 		///< Min Period Natural Freq
	int8_t sens_num;	///< Номер градуированного датчика
#if SCH_TYPE == 1
	int32_t duty_cycle; ///< PWR
#elif SCH_TYPE == 2
	int16_t att; 		///< Амлитуда возбуждения
	int16_t att0; 		///< Амплитуда возбуждения в отсутствие захвата
	uint8_t agc_on; 	///< Включение/Выключение АРУ
#endif
#ifdef AGC_ON
	int16_t agc_th;
#endif
	int32_t shift; 		///< phase shift
	edge_t edge; 		///< Edge Capture (0 - Rising Edge; 1 - Falling Edge)
	opmode_t mode; 		///< определено в OPMODE_T
	uint8_t filt_order; ///< Loop filter order
	uint8_t termo_src; 	///< Source Temperature (0 - Internal; 1 - External)
	uint8_t t;			///< Время работы, в тактах
	uint8_t agc_start; 	///< Одиночный запуск АРУ
	uint32_t amp; 		///< Огибающая вхоного сигнала
	int32_t T[2]; 		///< Current Period Natural Freq
	dpll_t * dpll;
	pack_t * pack;
}__attribute__((packed)) Preset_t;
/** @} */


void setFlgDataTr();
int getFlgDataTr();
void clearFlgDataTr();

void LED_On(uint32_t led);
void LED_Off(uint32_t led);
void LED_Blink(uint32_t led);
#if SCH_TYPE == 1
void AMP_Ctrl(uint16_t period);
#elif SCH_TYPE == 2
void AMP_Ctrl(int16_t period);
#endif

void cpu_boot();

int preset_Init();

void preset_Save();

#endif /* BOARD_H_ */

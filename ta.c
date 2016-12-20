/**
 * \file ta.c
 *
 * Тарировка модулей давления предназначена
 * для получения выходного значения давления, зависящего от
 * двух параметров (периода и температуры датчика).
 * Тарировка производится с на 9 точках температуры
 * рабочего диапазона (-60...+100 С°) с равномерным шагом 50 ГПа
 * в диапазоне  давлений 5...1350 ГПа (в начале диапазона 5, 8,
 * 50 далее с шагом 50 до 1350). Запись данных осуществляется,
 * последовательно после выдержки модуля на данной температуре
 * не менее 2 часов начиная с температуры -60.
 *
 *  Четырехточечная сплайн-интерполяция с постоянным шагом аргумента
 *
 *  Шаг аргумента = 1 * 2^14 = 16384
 *  Значение аргумента в первой точке = 0
 *  Разность значений функции в соседних точках не более 2^14 = 16384
 *  Число узлов (размер) интерполяционной таблицы не более 256
 */

#include "ta.h"
#include "uart.h"
#include "memo.h"
#include "xmodem.h"
#include "timer.h"

//---------------------------------------------------------------------------
Preset_t * preset;

//uint32_t NP[TA_NUM_SENSORS];   /* Код ЧП  */
//uint32_t VTPS[TA_NUM_SENSORS]; /* Код АЦП */

//Normal_Data Psen[TA_NUM_SENSORS];
DDG_Koef PSens1;

/**
 * Инициализация внутренних переменных
 */
void ta_Init(Preset_t * preset_)
{
	preset = preset_;

	preset->dpll->phase1 = 0;
	preset->dpll->period1 = 0;
	preset->dpll->phase2 = 0;
	preset->dpll->period2 = 0;
	preset->dpll->interp_valid_ta = 0;

	ta_InitTable();
}

/**
 * \brief Функция пересчета значения периода в давление (мм.рт.ст).
 *
 * Давление рассчитывается с помощью применения метода
 * двумерной интерполяции локальными кубическими сплайнами
 * по таблице значений функции с равноотстающими узлами.
 * Интерполяционная таблица  и коэффициенты масштабирования
 * являются результатами обработки тарировочных таблиц датчиков
 * давления, индивидуальны для каждого из них и хранятся в ПЗУ.
 *
 * \param[in] period - период, такт МК.
 * \param[in] temp - код значения температуры, МЗР.
 * \return P - код давления. Для перевода в мм.рт.ст необходимо P/256./1.33322368421052631578947368421016
 */
int32_t Calc_Pressures(int32_t T, int32_t U, uint8_t K)
{
	int32_t P;

	//	NP[0] = Form1->Edit1->Text.ToDouble()*2400;
	//	VTPS[0] = Form1->Edit2->Text.ToDouble()*10000;

	P = DDG(&T, &U, &K, &PSens1);          /* Calculate current pressure      */

	//               Form1->Edit3->Text=P;
	//               Form1->Edit4->Text=P/256./1.33322368421052631578947368421016; /* в мм рт. ст.*/
	return P;
}

/**
 * \brief Pressure calculation
 * Exec Time=6us @ 200MHz Fclk
 * Calculates pressure value by the data of FN-converters and voltage
 * from internal temperature sensor.
 * The procedure is developed for pressure transducers of DDG type.
 * Calculation based on 2-dimension local spline interpolation.
 *
 * Constants:
 * char DDG_BlockID[8] - Transducers block identifier
 * int16_t Tbl->DDG_TN[] - Numbers of nodes along T-axis
 * int32_t Tbl->DDG_T0[] - Minima of T-value
 * int32_t Tbl->DDG_TS[] - T multiplying coefficients
 * int16_t Tbl->DDG_TK[] - T scale factors (=2^TK)
 * int16_t Tbl->DDG_UN[] - Numbers of nodes along U-axis
 * int32_t Tbl->DDG_U0[] - Minima of U-value
 * int32_t Tbl->DDG_US[] -  U multiplying coefficients
 * int16_t Tbl->DDG_UK[] - U scale factors (=2^UK)
 * int16_t Tbl->DDG_OS[] - Offsets of interpolation tables in PA array
 * int32_t Tbl->DDG_PA[PASIZE] - Array of interpolation tables PASIZE=SUMM(TN[i]*UN[i]), i=1...6
 *
 * \param[in] T - Time period in system units (T=T[us]*96*25)
 * \param[in] U - Temperature sensor voltage in Volts scaled by 10000
 * \param[in] K - Transducer number;
 * \param[in] *Tbl - Адрес таблицы коэффициентов
 * \return P - Pressure in hPa scaled by 2^8
 */
int32_t DDG(int32_t * T, int32_t * U, uint8_t * K, DDG_Koef *Tbl)
{
	int32_t tt, uu, jt, zt, s, f[4], P;
	_INT INT;

	/* Arguments shifting and scaling */
	tt=(((*T)-Tbl->DDG_T0[*K])*Tbl->DDG_TS[*K]+(1<<(Tbl->DDG_TK[*K]-1)))>>Tbl->DDG_TK[*K];
	uu=(((*U)-Tbl->DDG_U0[*K])*Tbl->DDG_US[*K]+(1<<(Tbl->DDG_UK[*K]-1)))>>Tbl->DDG_UK[*K];
	/* Positioning within interpolation table */
	INT = indicINT(tt,Tbl->DDG_TN[*K]);
	jt = INT.j;
	zt = INT.z;
	INT = indicINT(uu,Tbl->DDG_UN[*K]);
	/* Interpolation */
	s = Tbl->DDG_OS[*K] + jt*Tbl->DDG_UN[*K] + INT.j;
	f[0] = fintINT(Tbl->DDG_PA + s, INT.z);
	s = s + Tbl->DDG_UN[*K];
	f[1] = fintINT(Tbl->DDG_PA + s, INT.z);
	s = s + Tbl->DDG_UN[*K];
	f[2] = fintINT(Tbl->DDG_PA + s, INT.z);
	s = s + Tbl->DDG_UN[*K];
	f[3] = fintINT(Tbl->DDG_PA + s, INT.z);
	P = fintINT(f, zt);
	/* Pressure scaling from 2^7 up to 2^8 */
	P <<= 1u;
	return P;
}

/**
 * Позиционирование в интерполяционной таблице.
 * \param [in] x значение аргумента : 2^14
 * \param [in] n число узлов (размер) интерполяционной таблицы
 * \return INT возвращаемая структура INT содержит переменные интерполяции:
 *		INT.j - Номер крайнего левого узла
 * 		INT.z - Расстояние до базового узла (обычно второго) : 2^14 = 16384
 */
_INT indicINT(int32_t x, int32_t n)
{
	_INT INT;
	INT.z = n - 3;
	INT.j = x >> 14;

	if(INT.j < 1)
	{
		INT.j = 1;
	}
	else if(INT.j > INT.z)
	{
		INT.j = INT.z;
	}

	INT.z = x-(INT.j << 14u);
	INT.j = INT.j - 1;

	if(INT.z < -16384)
	{
		INT.z = -16384;
	}
	else if(INT.z > 32768)
	{
		INT.z = 32768;
	}
	return INT;
}

/**
 *  Вычисление значения сплайна.
 *  \param [in] f значения функции в четырех ближайших узлах от INT.j до INT.j+3
 *  \param [in] x расстояние до базового узла (INT.z) : 2^14
 *  \return y значение сплайна.
 */
int32_t fintINT(const int32_t * f, int32_t x)
{
	int32_t y;
	if(x<=0) /* left   */
	{
		y=((((((f[0]-(f[1]<<1)+f[2])*x)>>14)+f[2]-f[0])*x)>>15)+f[1];
	}
	else if(x>=16384) /* right  */
	{
		y=x-16384;
		y=((((((f[1]-(f[2]<<1)+f[3])*y)>>14)+f[3]-f[1])*y)>>15)+f[2];
	}
	else /* middle */
	{
		y=(((((((((-f[0]+3*(f[1]-f[2])+f[3])*x)>>14)+(f[0]<<1)-5*f[1]
		                                                           +(f[2]<<2)-f[3])*x)>>14)+f[2]-f[0])*x)>>15)+f[1];
	}
	return y;
}

/**
 * Функция, возвращающая идентификатор датчика
 */
char * ta_SensID(uint8_t num)
{
	return PSens1.BlockID;
}

/**
 * Иициализация интерполяционной функции
 * для компенсации сдвигов фаз, вносимых датчиком.
 *
 * \param [in] point - номер точки, по которой будет рассчитываться интерполяция.
 * 		Возможные значение (TA_PHASE1_VALID_MSK, TA_PHASE2_VALID_MSK) из TA_VALID.
 */
uint8_t ta_InitPhaseInterpPoint(const int8_t point)
{
	uint8_t ret = FAILURE;

	if(point & TA_PHASE1_VALID_MSK)
	{
		preset->dpll->phase1 = preset->dpll->phase;
		preset->dpll->period1 = preset->T[(preset->t - 1) & 0x01]/preset->ave_num;
		preset->dpll->interp_valid_ta |= TA_PHASE1_VALID_MSK;

		memo_WriteByte(MEMO_CC1_REG, MEMO_TA_BANK_SEL, preset->dpll->phase1);

		ret = SUCCESS;
	}
	else if(point & TA_PHASE2_VALID_MSK)
	{
		preset->dpll->phase2 = preset->dpll->phase;
		preset->dpll->period2 = preset->T[(preset->t - 1) & 0x01]/preset->ave_num;
		preset->dpll->interp_valid_ta |= TA_PHASE2_VALID_MSK;

		memo_WriteByte(MEMO_CC2_REG, MEMO_TA_BANK_SEL, preset->dpll->phase2);

		ret = SUCCESS;
	}
	else if(point == 0)
	{
		preset->dpll->interp_valid_ta &= ~(TA_PHASE1_VALID_MSK | TA_PHASE2_VALID_MSK);
		memo_Erase(0, MEMO_TA_BANK_SEL);

		ret = SUCCESS;
	}

	if((preset->dpll->interp_valid_ta & (TA_PHASE1_VALID_MSK | TA_PHASE2_VALID_MSK)) ==
			(TA_PHASE1_VALID_MSK | TA_PHASE2_VALID_MSK))
	{
		memo_WriteByte(MEMO_DATA_KEY_REG, MEMO_TA_BANK_SEL, preset->dpll->interp_valid_ta);
	}

	return ret;
}

/**
 * \brief Линейная интерполяция фазового сдвига.
 *
 * Возвращает интерполированное значение фазового сдвига,
 * вносимого датчиком на резонансной частоте.
 * \param[in] period - сумма измерений периода, в тактах.
 * \param[out] ave - число накоплений.
 * \return phase - сдвиг фаз, в град.
 */
int32_t ta_PhaseInterp(const int32_t * period, const int32_t ave)
{
	int32_t phase;

	phase = ((preset->dpll->phase2 - preset->dpll->phase1) * (*period) +
			(preset->dpll->phase1 * preset->dpll->period2 -
			preset->dpll->phase2 * preset->dpll->period1) * ave) /
			((preset->dpll->period2 - preset->dpll->period1) * ave);

	return phase;
}

/**
 * \brief Загрузка тарировочной таблицы
 *
 * Загрузка данных тарировочной таблицы
 * производится через XMODEM.
 *
 *	\param[in] mode - режим работы до загрузки
 *	\return ret - результат: SUCCESS, FAILURE
 */
int8_t ta_Upload(opmode_t mode)
{
	int8_t ret;

	preset->mode = UPLOAD;

	TIMER_Cmd(MDR_TIMER1, DISABLE);

	xmoden_init();

	/* Прием данных по XMODEM */
	ret = xmodem_recv((uint8_t *)&PSens1, sizeof(DDG_Koef));
	/* Преобразование идентификатора в строку */
	PSens1.BlockID[TA_BLOCK_ID_LEN - 1] = 0;
	/* Запись данных тарировочной таблицы в ПЗУ */
	ta_WriteTable();
	/* Возврат в предыдущий режим работы */
	preset->mode = mode;

	Timer3_Configure(preset);

	TIMER_Cmd(MDR_TIMER1, ENABLE);

	return ret;
}

/**
 * \brief Выгрузка тарировочной таблицы
 *
 * Выгрузка данных тарировочной таблицы
 * производится через XMODEM.
 *
 *	\param[in] mode - режим работы до загрузки
 *	\return ret - результат: SUCCESS, FAILURE
 */
int8_t ta_Download(opmode_t mode)
{
	int8_t ret;

	preset->mode = DOWNLOAD;

	TIMER_Cmd(MDR_TIMER1, DISABLE);

	xmoden_init();

	/* Передача данных по XMODEM */
	ret = xmodem_trans((uint8_t *)&PSens1, sizeof(DDG_Koef));
	/* Возврат в предыдущий режим работы */
	preset->mode = mode;

	Timer3_Configure(preset);

	TIMER_Cmd(MDR_TIMER1, ENABLE);

	return ret;
}

/**
 * Функция записи тарировочных даннх в ПЗУ.
 */
void ta_WriteTable()
{
	memo_Erase(0, MEMO_TA_BANK_SEL);

	memo_Write(MEMO_DATA_ADDR, MEMO_TA_BANK_SEL, (uint8_t *)&PSens1, sizeof(DDG_Koef));

	memo_WriteByte(MEMO_DATA_KEY_REG, MEMO_TA_BANK_SEL, MEMO_DATA_KEY);
}

/**
 * \brief Функция инициализации тарировочной таблицы.
 *
 * Тарировочая таблица, хранящаяся в ОЗУ, инициализируется
 * данными из ПЗУ. Это сделано для ускорения работы программы, так как
 * скорость считывания данных из ПЗУ ограничена 40 нс (см. спецификацию на 1986ВЕ9х).
 */
void ta_InitTable()
{
	uint8_t key = 0;

	key = memo_ReadByte(MEMO_CC_KEY_REG, MEMO_TA_BANK_SEL);

	if(key == MEMO_CC_KEY)
	{

	}

	key = memo_ReadByte(MEMO_DATA_KEY_REG, MEMO_TA_BANK_SEL);

	if(key == MEMO_DATA_KEY)
	{
		memo_Read(MEMO_DATA_ADDR, MEMO_TA_BANK_SEL, (uint8_t *)&PSens1, sizeof(DDG_Koef));
	}
}

uint8_t ta_Valid()
{
	uint8_t key = 0;

	key = memo_ReadByte(MEMO_CC_KEY_REG, MEMO_TA_BANK_SEL);

	return key;
}

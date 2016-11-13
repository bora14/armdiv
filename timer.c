/*
 * timer.c
 *
 *  Created on: 29.11.2015
 *      Author: Ivan
 */


#include "timer.h"

#if SCH_TYPE == 1
int Timer1_Configure(Preset_t * preset)
{
	TIMER_CntInitTypeDef TIMER_CntInitStruct;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;

	TIMER_DeInit(MDR_TIMER1);

	TIMER_CntInitStruct.TIMER_IniCounter = 0;
	TIMER_CntInitStruct.TIMER_Prescaler  = TIMER1_Prescaler;
	TIMER_CntInitStruct.TIMER_Period     = preset->Tmax;
	TIMER_CntInitStruct.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;
	TIMER_CntInitStruct.TIMER_CounterDirection = TIMER_CntDir_Up;
	TIMER_CntInitStruct.TIMER_EventSource      = TIMER_EvSrc_None;
	TIMER_CntInitStruct.TIMER_FilterSampling   = TIMER_FDTS_TIMER_CLK_div_1;
	TIMER_CntInitStruct.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_On_CNT_Overflow;
	TIMER_CntInitStruct.TIMER_ETR_FilterConf   = TIMER_Filter_8FF_at_TIMER_CLK;
	TIMER_CntInitStruct.TIMER_ETR_Prescaler    = TIMER_ETR_Prescaler_None;
	TIMER_CntInitStruct.TIMER_ETR_Polarity     = TIMER_ETRPolarity_NonInverted;
	TIMER_CntInitStruct.TIMER_BRK_Polarity     = TIMER_BRKPolarity_NonInverted;

	TIMER_CntInit(MDR_TIMER1, &TIMER_CntInitStruct);

	/* ���������������� 1�� ������ ������� � ������ ��� */
	TIMER_ChnStructInit(&sTIM_ChnInit);

	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL1;
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena			  = ENABLE;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode 	  = TIMER_CH_CCR_Update_On_CNT_eq_0;

	if(preset->edge == Falling_Edge)
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
	else if(preset->edge == Rising_Edge)
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format7;
	else
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;

	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

	TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, preset->Tmax >> 1);
	TIMER_SetChnCompare1(MDR_TIMER1, TIMER_CHANNEL1, preset->Tmax - 1);

	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL1;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;

	TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

	/*************************************************/
	/* ���������������� 2�� ������ ������� � ������ ������ */

	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL2;
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_CAPTURE;
	sTIM_ChnInit.TIMER_CH_EventSource 		  = TIMER_CH_EvSrc_PE_OC1;// �� ��������� ������
	sTIM_ChnInit.TIMER_CH_FilterConf		  = TIMER_Filter_8FF_at_TIMER_CLK;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode 	  = TIMER_CH_CCR_Update_Immediately;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena			  = DISABLE;

	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL2;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Input;

	TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

	/* ���������������� 3�� ������ ������� � ������ ������ */

	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL3;
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_CAPTURE;
	sTIM_ChnInit.TIMER_CH_EventSource 		  = TIMER_CH_EvSrc_NE;// �� ������� ������
	sTIM_ChnInit.TIMER_CH_FilterConf		  = TIMER_Filter_8FF_at_TIMER_CLK;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode 	  = TIMER_CH_CCR_Update_Immediately;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena			  = DISABLE;

	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL3;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Input;

	TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

	/*************************************************/

	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	TIMER_Cmd(MDR_TIMER1, ENABLE);

	MDR_TIMER1->IE = 0x0;

	TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR | TIMER_STATUS_CCR_CAP_CH3, ENABLE);

	NVIC_SetPriority(Timer1_IRQn, TIMER1_EXP_PRIOR);

	NVIC_EnableIRQ(Timer1_IRQn);

	return 1;
}

int Timer3_Configure(Preset_t * preset)
{
	TIMER_CntInitTypeDef TIMER_CntInitStruct;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;

	/*************************************/

	TIMER_DeInit(AMP_Timer);

	TIMER_CntInitStruct.TIMER_IniCounter = 0;
	TIMER_CntInitStruct.TIMER_Prescaler  = TIMER3_Prescaler;
	TIMER_CntInitStruct.TIMER_Period     = PWR_CTRL_PERIOD << 1;
	TIMER_CntInitStruct.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;
	TIMER_CntInitStruct.TIMER_CounterDirection = TIMER_CntDir_Up;
	TIMER_CntInitStruct.TIMER_EventSource      = TIMER_EvSrc_None;
	TIMER_CntInitStruct.TIMER_FilterSampling   = TIMER_FDTS_TIMER_CLK_div_1;
	TIMER_CntInitStruct.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_On_CNT_Overflow;
	TIMER_CntInitStruct.TIMER_ETR_FilterConf   = TIMER_Filter_8FF_at_TIMER_CLK;
	TIMER_CntInitStruct.TIMER_ETR_Prescaler    = TIMER_ETR_Prescaler_None;
	TIMER_CntInitStruct.TIMER_ETR_Polarity     = TIMER_ETRPolarity_NonInverted;
	TIMER_CntInitStruct.TIMER_BRK_Polarity     = TIMER_BRKPolarity_NonInverted;

	TIMER_CntInit(AMP_Timer, &TIMER_CntInitStruct);

	/* ���������������� 1�� ������ ������� � ������ ��� */
	TIMER_ChnStructInit(&sTIM_ChnInit);

	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL3;
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena			  = ENABLE;

	TIMER_ChnInit(AMP_Timer, &sTIM_ChnInit);

	TIMER_SetChnCompare(AMP_Timer, TIMER_CHANNEL3, PWR_CTRL_PERIOD);
	TIMER_SetChnCompare1(AMP_Timer, TIMER_CHANNEL3, (PWR_CTRL_PERIOD << 1) - 1);

	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL3;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;

	TIMER_ChnOutInit(AMP_Timer, &sTIM_ChnOutInit);

	/*************************************************/

	TIMER_BRGInit(AMP_Timer,TIMER_HCLKdiv1);

	TIMER_Cmd(AMP_Timer, ENABLE);

	/*************************************/

	return 1;
}

#endif

#if SCH_TYPE == 2
int Timer1_Configure(Preset_t * preset)
{
	TIMER_CntInitTypeDef TIMER_CntInitStruct;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;

	TIMER_DeInit(MDR_TIMER1);

	TIMER_CntInitStruct.TIMER_IniCounter = 0;
	TIMER_CntInitStruct.TIMER_Prescaler  = TIMER1_Prescaler;
	TIMER_CntInitStruct.TIMER_Period     = preset->Tmax;
	TIMER_CntInitStruct.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;
	TIMER_CntInitStruct.TIMER_CounterDirection = TIMER_CntDir_Up;
	TIMER_CntInitStruct.TIMER_EventSource      = TIMER_EvSrc_None;
	TIMER_CntInitStruct.TIMER_FilterSampling   = TIMER_FDTS_TIMER_CLK_div_1;
	TIMER_CntInitStruct.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_On_CNT_Overflow;
	TIMER_CntInitStruct.TIMER_ETR_FilterConf   = TIMER_Filter_8FF_at_TIMER_CLK;
	TIMER_CntInitStruct.TIMER_ETR_Prescaler    = TIMER_ETR_Prescaler_None;
	TIMER_CntInitStruct.TIMER_ETR_Polarity     = TIMER_ETRPolarity_NonInverted;
	TIMER_CntInitStruct.TIMER_BRK_Polarity     = TIMER_BRKPolarity_NonInverted;

	TIMER_CntInit(MDR_TIMER1, &TIMER_CntInitStruct);

/*****************************************************/

	TIMER_ChnStructInit(&sTIM_ChnInit);

	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL1;
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena			  = DISABLE;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode 	  = TIMER_CH_CCR_Update_On_CNT_eq_0;
//	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode 	  = TIMER_CH_CCR_Update_Immediately;

	if(preset->edge == Falling_Edge)
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
	else if(preset->edge == Rising_Edge)
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format7;
	else
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;

	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL2;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena			  = ENABLE;

	if(preset->edge == Falling_Edge)
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format7;
	else if(preset->edge == Rising_Edge)
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
	else
		sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format7;

	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

	TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, preset->Tmax >> 1);
	TIMER_SetChnCompare1(MDR_TIMER1, TIMER_CHANNEL1, preset->Tmax - 1);

	TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL2, preset->Tmax >> 1);
	TIMER_SetChnCompare1(MDR_TIMER1, TIMER_CHANNEL2, preset->Tmax - 1);

	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL1;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;

	TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL2;

	TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

/******************************************************/

	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL3;
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_CAPTURE;
	sTIM_ChnInit.TIMER_CH_EventSource 		  = TIMER_CH_EvSrc_NE;
	sTIM_ChnInit.TIMER_CH_FilterConf		  = TIMER_Filter_1FF_at_TIMER_CLK;//TIMER_Filter_8FF_at_FTDS_div_32;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode 	  = TIMER_CH_CCR_Update_Immediately;
//	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode 	  = TIMER_CH_CCR_Update_On_CNT_eq_0;
	sTIM_ChnInit.TIMER_CH_CCR1_Ena			  = DISABLE;

	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

	sTIM_ChnOutInit.TIMER_CH_Number = TIMER_CHANNEL3;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Input;

	TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

	/*************************************************/

	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	TIMER_Cmd(MDR_TIMER1, ENABLE);

	MDR_TIMER1->IE = 0x0;

	TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR | TIMER_STATUS_CCR_CAP_CH3, ENABLE);

	NVIC_SetPriority(Timer1_IRQn, TIMER1_EXP_PRIOR);

	NVIC_EnableIRQ(Timer1_IRQn);

	return 1;
}

int Timer3_Configure(Preset_t * preset)
{
	TIMER_CntInitTypeDef TIMER_CntInitStruct;

	/*************************************/

	TIMER_DeInit(MDR_TIMER3);

	TIMER_CntInitStruct.TIMER_IniCounter 		= 0;
	TIMER_CntInitStruct.TIMER_Prescaler 		= TIMER3_Prescaler;
	TIMER_CntInitStruct.TIMER_Period     		= AGC_FREQ;
	TIMER_CntInitStruct.TIMER_CounterMode      	= TIMER_CntMode_ClkFixedDir;
	TIMER_CntInitStruct.TIMER_CounterDirection 	= TIMER_CntDir_Up;
	TIMER_CntInitStruct.TIMER_EventSource      	= TIMER_EvSrc_None;
	TIMER_CntInitStruct.TIMER_FilterSampling   	= TIMER_FDTS_TIMER_CLK_div_1;
	TIMER_CntInitStruct.TIMER_ARR_UpdateMode   	= TIMER_ARR_Update_On_CNT_Overflow;
	TIMER_CntInitStruct.TIMER_ETR_FilterConf   	= TIMER_Filter_8FF_at_TIMER_CLK;
	TIMER_CntInitStruct.TIMER_ETR_Prescaler    	= TIMER_ETR_Prescaler_None;
	TIMER_CntInitStruct.TIMER_ETR_Polarity     	= TIMER_ETRPolarity_NonInverted;
	TIMER_CntInitStruct.TIMER_BRK_Polarity     	= TIMER_BRKPolarity_NonInverted;

	TIMER_CntInit(MDR_TIMER3, &TIMER_CntInitStruct);
	/*************************************************/

	TIMER_BRGInit(MDR_TIMER3,TIMER_HCLKdiv1);

	TIMER_Cmd(MDR_TIMER3, ENABLE);

	MDR_TIMER3->IE = 0x0;

	TIMER_ITConfig(MDR_TIMER3, TIMER_STATUS_CNT_ARR, ENABLE);

	NVIC_SetPriority(Timer3_IRQn, TIMER3_EXP_PRIOR);

	NVIC_EnableIRQ(Timer3_IRQn);

	/*************************************/

	return 1;
}

#endif

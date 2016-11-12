/**
  ******************************************************************************
  * @file    system_MDR32F9Qx.c
  * @author  Phyton Application Team
  * @version V1.0.0
  * @date    11/06/2010
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Source File.
  ******************************************************************************
  * <br><br>
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, PHYTON SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 Phyton</center></h2>
  ******************************************************************************
  * FILE system_MDR32F9Qx.c
  */


/** @addtogroup __CMSIS CMSIS
  * @{
  */

/** @addtogroup __MDR32F9QX MDR32F9QX System
  * @{
  */

/** @addtogroup System_Private_Includes System Private Includes
  * @{
  */

#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
/** @} */ /* End of group System_Private_Includes */

/** @addtogroup __MDR32F9QX_System_Private_Variables MDR32F9QX System Private Variables
  * @{
  */

/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
  uint32_t SystemCoreClock = (uint32_t)CPU_MCK;         /*!< System Clock Frequency (Core Clock)
                                                         *   default value */

/** @} */ /* End of group __MDR32F9QX_System_Private_Variables */

/** @addtogroup __MDR32F9QX_System_Private_Functions MDR32F9QX System Private Functions
  * @{
  */

/**
  * @brief  Update SystemCoreClock according to Clock Register Values
  * @note   None
  * @param  None
  * @retval None
  */
#include "MDR32F9Qx_rst_clk.h"

void SystemCoreClockUpdate (void)
{
	uint32_t cpu_c1_freq, cpu_c2_freq, cpu_c3_freq;
	uint32_t cpu_c1_src;
	uint32_t pll_mul;

	cpu_c1_src = RST_CLK_CPU_PLLsrcHSEdiv1;
	cpu_c1_freq = HSE_Value;

	RST_CLK_HSEconfig(RST_CLK_HSE_ON);

	if(RST_CLK_HSEstatus() != SUCCESS)
	{
		cpu_c1_src = RST_CLK_CPU_PLLsrcHSIdiv1;
		cpu_c1_freq = HSI_Value;
	}

	RST_CLK_CPU_PLLcmd(ENABLE);

	pll_mul = SystemCoreClock/cpu_c1_freq - 1;

	RST_CLK_CPU_PLLconfig(cpu_c1_src, pll_mul);

	cpu_c2_freq = cpu_c1_freq * pll_mul;

	if(RST_CLK_CPU_PLLstatus() != SUCCESS)
	{
		cpu_c2_freq = cpu_c1_freq;
	}

	RST_CLK_CPU_PLLuse(ENABLE);

	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);

	cpu_c3_freq = cpu_c2_freq;

	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);

	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

//	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP1, ENABLE);

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_IWDG, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);
//	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER3, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_I2C, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
#ifdef DMA
	RST_CLK_PCLKcmd(RST_CLK_PCLK_DMA, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP1, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP2, ENABLE);
#endif

#ifndef RELEASE
	SCB->VTOR = 0x20000000;
	SCB->SCR |= SCB_SCR_SEVONPEND_Msk;
#else
	SCB->VTOR = 0x08000000;
#endif
}

/**
  * @brief  Setup the microcontroller system
  *         RST clock configuration to the default reset state
  *         Setup SystemCoreClock variable.
  * @note   This function should be used only after reset.
  * @param  None
  * @retval None
  */
void SystemInit (void)
{
	/* Reset the RST clock configuration to the default reset state */

  /* Reset all clock but RST_CLK & BKP_CLC bits */
  MDR_RST_CLK->PER_CLOCK   = (uint32_t)0x8000010;

  /* Reset CPU_CLOCK bits */
  MDR_RST_CLK->CPU_CLOCK   &= (uint32_t)0x00000000;

  /* Reset PLL_CONTROL bits */
  MDR_RST_CLK->PLL_CONTROL &= (uint32_t)0x00000000;

  /* Reset HSEON and HSEBYP bits */
  MDR_RST_CLK->HS_CONTROL  &= (uint32_t)0x00000000;

  /* Reset USB_CLOCK bits */
  MDR_RST_CLK->USB_CLOCK   &= (uint32_t)0x00000000;

  /* Reset ADC_MCO_CLOCK bits */
  MDR_RST_CLK->ADC_MCO_CLOCK   &= (uint32_t)0x00000000;

  SystemCoreClockUpdate();
}

/** @} */ /* End of group __MDR32F9QX_System_Private_Functions */

/** @} */ /* End of group __MDR32F9QX */

/** @} */ /* End of group __CMSIS */

/******************* (C) COPYRIGHT 2010 Phyton *********************************
*
* END OF FILE system_MDR32F9Qx.c */

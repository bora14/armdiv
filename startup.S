/**
  ******************************************************************************
  * @file      startup_stm32f2xx.s
  * @author    MCD Application Team
  * @version   V1.1.3
  * @date      05-March-2012
  * @brief     STM32F2xx Devices vector table for Atollic TrueSTUDIO toolchain.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Configure the system clock and the external SRAM mounted on
  *                  STM322xG-EVAL board to be used as data memory (optional,
  *                  to be enabled by user)
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M3 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

    .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:

/* Copy the data segment initializers from flash to SRAM */
  movs  r1, #0
  b  LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit
  ldr  r2, =_sbss
  b  LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss
/* Call the clock system intitialization function.*/
  bl  SystemInit
/* Call static constructors */
/*  bl __libc_init_array */
/* Call the application's entry point.*/
  bl  main
  bx  lr
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None
 * @retval None
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
*******************************************************************************/
   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler

  /* External Interrupts */
  .word     CAN1_IRQHandler            // IRQ0
  .word     CAN2_IRQHandler            // IRQ1
  .word     USB_IRQHandler             // IRQ2
  .word     0                          // IRQ3  reserved
  .word     0                          // IRQ4  reserved
  .word     DMA_IRQHandler             // IRQ5
  .word     UART1_IRQHandler           // IRQ6
  .word     UART2_IRQHandler           // IRQ6
  .word     SSP1_IRQHandler            // IRQ8
  .word     0                          // IRQ9  reserved
  .word     I2C_IRQHandler             // IRQ10
  .word     POWER_IRQHandler           // IRQ11
  .word     WWDG_IRQHandler            // IRQ12
  .word     0                          // IRQ13 reserved
  .word     Timer1_IRQHandler          // IRQ14
  .word     Timer2_IRQHandler          // IRQ15
  .word     Timer3_IRQHandler          // IRQ16
  .word     ADC_IRQHandler             // IRQ17
  .word     0                          // IRQ18 reserved
  .word     COMPARATOR_IRQHandler      // IRQ19
  .word     SSP2_IRQHandler            // IRQ20
  .word     0                          // IRQ21 reserved
  .word     0                          // IRQ22 reserved
  .word     0                          // IRQ23 reserved
  .word     0                          // IRQ24 reserved
  .word     0                          // IRQ25 reserved
  .word     0                          // IRQ26 reserved
  .word     BACKUP_IRQHandler          // IRQ27
  .word     EXT_INT1_IRQHandler        // IRQ28
  .word     EXT_INT2_IRQHandler        // IRQ29
  .word     EXT_INT3_IRQHandler        // IRQ30
  .word     EXT_INT4_IRQHandler        // IRQ31


/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/
   .weak      NMI_Handler
   .thumb_set NMI_Handler,Default_Handler

   .weak      HardFault_Handler
   .thumb_set HardFault_Handler,Default_Handler

   .weak      MemManage_Handler
   .thumb_set MemManage_Handler,Default_Handler

   .weak      BusFault_Handler
   .thumb_set BusFault_Handler,Default_Handler

   .weak      UsageFault_Handler
   .thumb_set UsageFault_Handler,Default_Handler

   .weak      SVC_Handler
   .thumb_set SVC_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      PendSV_Handler
   .thumb_set PendSV_Handler,Default_Handler

   .weak      SysTick_Handler
   .thumb_set SysTick_Handler,Default_Handler

   .weak      CAN1_IRQHandler
   .thumb_set CAN1_IRQHandler,Default_Handler

   .weak      CAN2_IRQHandler
   .thumb_set CAN2_IRQHandler,Default_Handler

   .weak      USB_IRQHandler
   .thumb_set USB_IRQHandler,Default_Handler

   .weak      DMA_IRQHandler
   .thumb_set DMA_IRQHandler,Default_Handler

   .weak      UART1_IRQHandler
   .thumb_set UART1_IRQHandler,Default_Handler

   .weak      UART2_IRQHandler
   .thumb_set UART2_IRQHandler,Default_Handler

   .weak      SSP1_IRQHandler
   .thumb_set SSP1_IRQHandler,Default_Handler

   .weak      I2C_IRQHandler
   .thumb_set I2C_IRQHandler,Default_Handler

   .weak      POWER_IRQHandler
   .thumb_set POWER_IRQHandler,Default_Handler

   .weak      WWDG_IRQHandler
   .thumb_set WWDG_IRQHandler,Default_Handler

   .weak      Timer1_IRQHandler
   .thumb_set Timer1_IRQHandler,Default_Handler

   .weak      Timer2_IRQHandler
   .thumb_set Timer2_IRQHandler,Default_Handler

   .weak      Timer3_IRQHandler
   .thumb_set Timer3_IRQHandler,Default_Handler

   .weak      ADC_IRQHandler
   .thumb_set ADC_IRQHandler,Default_Handler

   .weak      COMPARATOR_IRQHandler
   .thumb_set COMPARATOR_IRQHandler,Default_Handler

   .weak      SSP2_IRQHandler
   .thumb_set SSP2_IRQHandler,Default_Handler

   .weak      BACKUP_IRQHandler
   .thumb_set BACKUP_IRQHandler,Default_Handler

   .weak      EXT_INT1_IRQHandler
   .thumb_set EXT_INT1_IRQHandler,Default_Handler

   .weak      EXT_INT2_IRQHandler
   .thumb_set EXT_INT2_IRQHandler,Default_Handler

   .weak      EXT_INT3_IRQHandler
   .thumb_set EXT_INT3_IRQHandler,Default_Handler

   .weak      EXT_INT4_IRQHandler
   .thumb_set EXT_INT4_IRQHandler,Default_Handler

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

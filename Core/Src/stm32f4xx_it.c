/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
int running_flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void PendSV_Handler(void)__attribute__ (( naked ));
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */
  extern int task1_stack[100];
  void LED_PF9(void);
  task1_stack[8] = 0xfffffffd; // r14
  task1_stack[9] = (int)&task1_stack[92];
  task1_stack[98] = (int)LED_PF9; // pc
  task1_stack[99] = 0x1000000; // xPSR

  extern int task2_stack[100];
  void LED_PF10(void);
  task2_stack[8] = 0xfffffffd;
  task2_stack[9] = (int)&task2_stack[92];
  task2_stack[98] = (int)LED_PF10;
  task2_stack[99] = 0x1000000;

  running_flag = 1;
  while (1)
  {
    __asm("ldr r0,=0xe000ed08 \n"
          "ldr r0,[r0] \n"
          "ldr r0,[r0] \n"
          "msr msp,r0"); // 销毁主栈区

    __asm("ldr r0,=task1_stack_top");
    __asm("ldr r0,[r0]");
    __asm("msr psp,r0"); // 默认栈顶地址为[92]

    __asm("ldr r0,=task1_stack \n"
          "ldmia r0!,{r4-r11,r14} \n");
    __asm("bx r14");
    // 自动出栈R0 R1 R2 R3 R12 LR PC xPSR
  }
  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
/* USER CODE BEGIN PendSV_IRQn 0 */
  #define jump_to_task2 0
  #define jump_to_task1 1
  static int jump_flag = 0;
  if (jump_flag == jump_to_task2) // 跳到task2
  {
	 // stmdb 先-再保存   同入栈
   // mrs 后面的复制到前面
   // str 前面的存到后面
   // ldr 后面的写到前面
   // ldmia 先保存后+   同出栈
    jump_flag = jump_to_task1;
    __asm("ldr r0,= task1_psp_r4_r11_r14_save_pointer"); // 保存stack1 psp_r4_r11_r14
    __asm("ldr r0,[r0]");
    __asm("mrs r1,psp");
    __asm("str r1,[r0]");
    __asm("stmdb r0!,{r4-r11,r14}");

    __asm("ldr r0,=task2_stack \n" // 释放stack2 r4_r11_r14
          "ldmia r0!,{r4-r11,r14} \n");
    __asm("ldr r0,[r0]");
    __asm("msr psp,r0"); // 载入psp
    __asm("bx r14");
  }
  else // 跳到task1
  {
	  jump_flag = jump_to_task2;
    __asm("ldr r0,= task2_psp_r4_r11_r14_save_pointer");
    __asm("ldr r0,[r0]");
    __asm("mrs r1,psp"); // 保存psp
    __asm("str r1,[r0]");
    __asm("stmdb r0!,{r4-r11,r14}"); // 继续保存r4-r11 r14

    __asm("ldr r0,=task1_stack \n" // 释放stack1 r4_r11_r14
          "ldmia r0!,{r4-r11,r14} \n");
    __asm("ldr r0,[r0]");
    __asm("msr psp,r0"); // 载入psp
    __asm("bx r14");
  }

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	  if (!running_flag)
	    return;
  (*((volatile uint32_t *)0xe000ed04)) = 1 << 28;
  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
 */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  ******************************************************************************
  * @file    openamp_driver.c
  * @author  MCD Application Team
  * @brief   OpenAMP platform driver Implementation.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/**
  * @addtogroup OpenampDriver
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "openamp_driver.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* MPU regions declaration defined in the Linker script */
extern int __MPU_CODE_region_start__[];
extern int __MPU_CODE_region_end__[];
extern int __MPU_DATA_region_start__[];
extern int __MPU_DATA_region_end__[];
extern int __MPU_SHARED_NC_region_start__[];
extern int __MPU_SHARED_NC_region_end__[];

/* Exported variables --------------------------------------------------------*/
/**
  * @brief Global IPCC handle used by OpenAMP transport.
  */
IPCC_HandleTypeDef hipcc;

/* Private function prototypes -----------------------------------------------*/
static void OpenampDriver_ConfigMpuRegion(void);
static int OpenampDriver_IPCCInit(void);
static int OpenampDriver_Init(void);
static void OpenampDriver_Deinit(void);

/* Exported driver instance --------------------------------------------------*/
OpenampDriverTypeDef openampDriver =
{
  .init = OpenampDriver_Init,
  .deinit = OpenampDriver_Deinit,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure MPU regions required by OpenAMP transport.
  * @note   MPU regions boundaries are provided by the linker script.
  * @retval None
  */
static void OpenampDriver_ConfigMpuRegion(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  MPU_Attributes_InitTypeDef MPU_Attributes_InitStruct;

  HAL_MPU_Disable();

  /* Code Section (FLASH): write back, read allocate */
  MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_WRITE_BACK | MPU_NON_TRANSIENT | MPU_R_ALLOCATE);
  MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RO;
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.BaseAddress = (uint32_t)__MPU_CODE_region_start__;
  MPU_InitStruct.LimitAddress = (uint32_t)__MPU_CODE_region_end__;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Data Section (RAM): write back, read and write allocate */
  MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_WRITE_BACK | MPU_TRANSIENT | MPU_RW_ALLOCATE);
  MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.Enable = MPU_REGION_DISABLE; /* Kept disabled to avoid TF-M runtime faults when MPU is configured for this region. */
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER1;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.BaseAddress = (uint32_t)__MPU_DATA_region_start__;
  MPU_InitStruct.LimitAddress = (uint32_t)__MPU_DATA_region_end__;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Shared memory: IPC_SHMEM_1 & VIRTIO_SHMEM - non cacheable */
  MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
  MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER2;
  HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER2;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
  MPU_InitStruct.BaseAddress = (uint32_t)__MPU_SHARED_NC_region_start__;
  MPU_InitStruct.LimitAddress = (uint32_t)__MPU_SHARED_NC_region_end__;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  Initialize the IPCC peripheral and enable required interrupts.
  * @retval 0 on success, -1 otherwise
  */
static int OpenampDriver_IPCCInit(void)
{
  if (hipcc.Instance == NULL)
  {
    hipcc.Instance = IPCC1;
  }

  if (HAL_IPCC_Init(&hipcc) != HAL_OK)
  {
    return -1;
  }

  /* IPCC interrupt Init */
  HAL_NVIC_SetPriority(IPCC1_RX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(IPCC1_RX_IRQn);

  return 0;
}

/**
  * @brief  Initialize the OpenAMP platform driver.
  * @retval 0 on success, -1 otherwise
  */
static int OpenampDriver_Init(void)
{
  OpenampDriver_ConfigMpuRegion();

  if (OpenampDriver_IPCCInit() != 0)
  {
    return -1;
  }

  return 0;
}

/**
  * @brief  De-initialize the OpenAMP platform driver.
  * @retval None
  */
static void OpenampDriver_Deinit(void)
{
  HAL_NVIC_DisableIRQ(IPCC1_RX_IRQn);

  if (hipcc.Instance != NULL)
  {
    (void)HAL_IPCC_DeInit(&hipcc);
    hipcc.Instance = NULL;
  }
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

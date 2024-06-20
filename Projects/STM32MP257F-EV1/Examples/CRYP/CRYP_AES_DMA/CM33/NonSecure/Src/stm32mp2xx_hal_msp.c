/**
  ******************************************************************************
  * @file    stm32mp1xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "res_mgr.h"

DMA_HandleTypeDef hdmaIn;
DMA_HandleTypeDef hdmaOut;

extern void Error_Handler(void);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * Initializes the CRYP MSP.
  */
void HAL_CRYP_MspInit(CRYP_HandleTypeDef* hcryp)
{

  if(hcryp->Instance==CRYP2)
  {
	  /* USER CODE BEGIN CRYP_MspInit 0 */

	  /* USER CODE END CRYP_MspInit 0 */

    /* Peripheral CRYP2 clock enable */
    __HAL_RCC_CRYP2_CLK_ENABLE();

	  /* Set hmdmaIn and hmdmaOut memory bloc at 0 */
	  memset(&hdmaIn,  0, sizeof(DMA_HandleTypeDef));
	  memset(&hdmaOut, 0, sizeof(DMA_HandleTypeDef));

	 /***************** Configure HPDMA3 IN for CRYP2 ************************/
	  hdmaIn.Init.Request               = HPDMA_REQUEST_CRYP2_IN;
	  hdmaIn.Init.BlkHWRequest          = DMA_BREQ_SINGLE_BURST;
	  hdmaIn.Init.Direction             = DMA_MEMORY_TO_PERIPH;
	  hdmaIn.Init.SrcInc                = DMA_SINC_INCREMENTED;
	  hdmaIn.Init.DestInc               = DMA_DINC_FIXED;
	  hdmaIn.Init.SrcDataWidth          = DMA_SRC_DATAWIDTH_WORD;
	  hdmaIn.Init.DestDataWidth         = DMA_DEST_DATAWIDTH_WORD;
	  hdmaIn.Init.SrcBurstLength        = 4;
	  hdmaIn.Init.DestBurstLength       = 4;
	  hdmaIn.Init.Priority              = DMA_HIGH_PRIORITY;
	  hdmaIn.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
	  hdmaIn.Init.TransferEventMode     = DMA_TCEM_BLOCK_TRANSFER;
	  hdmaIn.Init.Mode                  = DMA_NORMAL;
	  hdmaIn.Instance                   = HPDMA3_Channel1;

	  /* Deinitialize the Stream for new transfer */
	  HAL_DMA_DeInit(hcryp->hdmain);

	  /* Associate the DMA handle */
	  __HAL_LINKDMA(hcryp, hdmain, hdmaIn);

	  /* Configure the MDMA Stream */
	  HAL_DMA_Init(hcryp->hdmain);

	  /* NVIC configuration for GPDMA Input data interrupt */
	  HAL_NVIC_SetPriority(HPDMA3_Channel1_IRQn, 0, 1);
	  HAL_NVIC_EnableIRQ(HPDMA3_Channel1_IRQn);

	  /***************** Configure HPDMA3 OUT for CRYP2 ***********************/
	  hdmaOut.Init.Request               = HPDMA_REQUEST_CRYP2_OUT;
	  hdmaOut.Init.BlkHWRequest          = DMA_BREQ_SINGLE_BURST;
	  hdmaOut.Init.Direction             = DMA_PERIPH_TO_MEMORY;
	  hdmaOut.Init.SrcInc                = DMA_SINC_FIXED;
	  hdmaOut.Init.DestInc               = DMA_DINC_INCREMENTED;
	  hdmaOut.Init.SrcDataWidth          = DMA_SRC_DATAWIDTH_WORD;
	  hdmaOut.Init.DestDataWidth         = DMA_DEST_DATAWIDTH_WORD;
	  hdmaOut.Init.SrcBurstLength        = 4;
	  hdmaOut.Init.DestBurstLength       = 4;
	  hdmaOut.Init.Priority              = DMA_HIGH_PRIORITY;
	  hdmaOut.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
	  hdmaOut.Init.TransferEventMode     = DMA_TCEM_BLOCK_TRANSFER;
	  hdmaIn.Init.Mode                   = DMA_NORMAL;

	  hdmaOut.Instance                   = HPDMA3_Channel2;

	  /* Deinitialize the Stream for new processing */
	  HAL_DMA_DeInit(hcryp->hdmaout);

	  /* Associate the DMA handle */
	  __HAL_LINKDMA(hcryp, hdmaout, hdmaOut);

	  /* Configure the DMA Stream */
	  HAL_DMA_Init(hcryp->hdmaout);

	  /* NVIC configuration for DMA output data interrupt */
	  HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);
  /* USER CODE BEGIN CRYP_MspInit 1 */

  /* USER CODE END CRYP_MspInit 1 */
  }

}

/**
  * Disable the CRYP MSP.
  */
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef* hcryp)
{
  /* USER CODE BEGIN CRYP_MspDeInit 0 */

  /* USER CODE END CRYP_MspDeInit 0 */

  /* Peripheral clock disable */
  __HAL_RCC_CRYP2_CLK_DISABLE();

  /* USER CODE BEGIN CRYP_MspDeInit 1 */

  /* USER CODE END CRYP_MspDeInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

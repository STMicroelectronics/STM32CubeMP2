/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization
  *                      and de-Initialization codes.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "res_mgr.h"

extern DMA_HandleTypeDef handle_HPDMA3_Channel2;

extern DMA_HandleTypeDef handle_HPDMA3_Channel1;

extern DMA_HandleTypeDef handle_HPDMA3_Channel0;

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
  * @brief I3C MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hi3c: I3C handle pointer
  * @retval None
  */
void HAL_I3C_MspInit(I3C_HandleTypeDef *hi3c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (hi3c->Instance == I3C1)
  {
    /* USER CODE BEGIN I3C1_MspInit 0 */

    /* USER CODE END I3C1_MspInit 0 */

    __HAL_RCC_I3C1_CLK_ENABLE();
    __HAL_RCC_I3C1_FORCE_RESET();
    __HAL_RCC_I3C1_RELEASE_RESET();

    /* I3C1 DMA Init */
    /* HPDMA3_REQUEST_I3C1_TX Init */
    handle_HPDMA3_Channel1.Instance = HPDMA3_Channel1;
    handle_HPDMA3_Channel1.Init.Request = HPDMA_REQUEST_I3C1_TX;
    handle_HPDMA3_Channel1.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_HPDMA3_Channel1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    handle_HPDMA3_Channel1.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_HPDMA3_Channel1.Init.DestInc = DMA_DINC_FIXED;
    handle_HPDMA3_Channel1.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_HPDMA3_Channel1.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_HPDMA3_Channel1.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_HPDMA3_Channel1.Init.SrcBurstLength = 1;
    handle_HPDMA3_Channel1.Init.DestBurstLength = 1;
    handle_HPDMA3_Channel1.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
    handle_HPDMA3_Channel1.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_HPDMA3_Channel1.Init.Mode = DMA_NORMAL;
    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&handle_HPDMA3_Channel1);

    /* Configure the DMA Stream */
    if (HAL_DMA_Init(&handle_HPDMA3_Channel1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi3c, hdmatx, handle_HPDMA3_Channel1);

    if (IS_DEVELOPER_BOOT_MODE())
    {
      HAL_DMA_ConfigChannelAttributes(hi3c->hdmatx,
                                      (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | \
                                       DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
    }

    /* HPDMA3_REQUEST_I3C1_RX Init */
    handle_HPDMA3_Channel2.Instance = HPDMA3_Channel2;
    handle_HPDMA3_Channel2.Init.Request = HPDMA_REQUEST_I3C1_RX;
    handle_HPDMA3_Channel2.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_HPDMA3_Channel2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    handle_HPDMA3_Channel2.Init.SrcInc = DMA_SINC_FIXED;
    handle_HPDMA3_Channel2.Init.DestInc = DMA_DINC_INCREMENTED;
    handle_HPDMA3_Channel2.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_HPDMA3_Channel2.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_HPDMA3_Channel2.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_HPDMA3_Channel2.Init.SrcBurstLength = 1;
    handle_HPDMA3_Channel2.Init.DestBurstLength = 1;
    handle_HPDMA3_Channel2.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT1 | DMA_DEST_ALLOCATED_PORT0;
    handle_HPDMA3_Channel2.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_HPDMA3_Channel2.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_HPDMA3_Channel2) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(hi3c, hdmarx, handle_HPDMA3_Channel2);

    if (IS_DEVELOPER_BOOT_MODE())
    {
      HAL_DMA_ConfigChannelAttributes(hi3c->hdmarx,
                                      (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | \
                                       DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
    }

    /* HPDMA3_REQUEST_I3C1_TC Init */
    handle_HPDMA3_Channel0.Instance = HPDMA3_Channel0;
    handle_HPDMA3_Channel0.Init.Request = HPDMA_REQUEST_I3C1_CMD;
    handle_HPDMA3_Channel0.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_HPDMA3_Channel0.Init.Direction = DMA_MEMORY_TO_PERIPH;
    handle_HPDMA3_Channel0.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_HPDMA3_Channel0.Init.DestInc = DMA_DINC_FIXED;
    handle_HPDMA3_Channel0.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
    handle_HPDMA3_Channel0.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    handle_HPDMA3_Channel0.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_HPDMA3_Channel0.Init.SrcBurstLength = 1;
    handle_HPDMA3_Channel0.Init.DestBurstLength = 1;
    handle_HPDMA3_Channel0.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
    handle_HPDMA3_Channel0.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_HPDMA3_Channel0.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_HPDMA3_Channel0) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(hi3c, hdmacr, handle_HPDMA3_Channel0);
    if (IS_DEVELOPER_BOOT_MODE())
    {
      HAL_DMA_ConfigChannelAttributes(hi3c->hdmacr,
                                      (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | \
                                       DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
    }


    /* I3C1 interrupt Init */
    HAL_NVIC_SetPriority(I3C1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I3C1_IRQn);

    /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(HPDMA3_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel0_IRQn);
    HAL_NVIC_SetPriority(HPDMA3_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel1_IRQn);
    HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);

    /* Add a delay to let startup of High level on the Bus */
    HAL_Delay(1);

    /* Then set the pull at default value for I3C communication, mean no pull */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  }
  /* USER CODE BEGIN I3C1_MspInit 1 */
  /* USER CODE END I3C1_MspInit 1 */

}

void HAL_I3C_MspPostInit(I3C_HandleTypeDef *hi3c)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* USER CODE BEGIN I2C_MspPostInit 0 */

  /* USER CODE END I2C_MspPostInit 0 */
  if (I3C1 == hi3c->Instance)
  {
    /**I3C1 GPIO Configuration
    PG13    ------> I3C1_SCL
    PA2     ------> I3C1_SDA
      */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_I3C1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_I3C1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  }
}

/**
  * @brief I3C MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hi3c: I3C handle pointer
  * @retval None
  */
void HAL_I3C_MspDeInit(I3C_HandleTypeDef *hi3c)
{
  if (I3C1 == hi3c->Instance)
  {
    /* USER CODE BEGIN I3C1_MspDeInit 0 */

    /* USER CODE END I3C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I3C1_CLK_DISABLE();

    /**I3C1 GPIO Configuration
    PG13    ------> I3C1_SCL
    PA2     ------> I3C1_SDA
      */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_13);

    /* I3C1 DMA DeInit */
    HAL_DMA_DeInit(hi3c->hdmatx);
    HAL_DMA_DeInit(hi3c->hdmarx);
    HAL_DMA_DeInit(hi3c->hdmacr);

    /* I3C1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I3C1_IRQn);

    /* USER CODE BEGIN I3C1_MspDeInit 1 */
    /* USER CODE END I3C1_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */


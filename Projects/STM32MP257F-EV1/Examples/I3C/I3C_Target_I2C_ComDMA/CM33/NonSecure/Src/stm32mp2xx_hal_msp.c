/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

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

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  if(I2C8 == hi2c->Instance)
  {
    /* USER CODE BEGIN I2C8_MspInit 0 */
    /* USER CODE END I2C8_MspInit 0 */

    __HAL_RCC_I2C8_CLK_ENABLE();
    __HAL_RCC_I2C8_FORCE_RESET();
    __HAL_RCC_I2C8_RELEASE_RESET();

    /* I2C8 DMA Init */
    /* I2C8_TX Init */
    hdma_tx.Instance = HPDMA3_Channel2;
    hdma_tx.Init.Request = HPDMA_REQUEST_I2C8_TX;
    hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.SrcInc = DMA_SINC_INCREMENTED;
    hdma_tx.Init.DestInc = DMA_DINC_FIXED;
    hdma_tx.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    hdma_tx.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    hdma_tx.Init.SrcBurstLength = 1;
    hdma_tx.Init.DestBurstLength = 1;
    hdma_tx.Init.Priority = DMA_HIGH_PRIORITY;
    hdma_tx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_tx);

    /* Configure the DMA Stream */
    if (HAL_OK != HAL_DMA_Init(&hdma_tx))
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi2c, hdmatx, hdma_tx);

    /* I2C8_RX Init */
    hdma_rx.Instance = HPDMA3_Channel3;
    hdma_rx.Init.Request = HPDMA_REQUEST_I2C8_RX;
    hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.SrcInc = DMA_SINC_FIXED;
    hdma_rx.Init.DestInc = DMA_DINC_INCREMENTED;
    hdma_rx.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    hdma_rx.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    hdma_rx.Init.SrcBurstLength = 1;
    hdma_rx.Init.DestBurstLength = 1;
    hdma_rx.Init.Priority = DMA_HIGH_PRIORITY;
    hdma_rx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;

    HAL_DMA_DeInit(&hdma_rx);
    if (HAL_OK != HAL_DMA_Init(&hdma_rx))
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi2c, hdmarx, hdma_rx);

    /* I2C8 interrupt Init */
    HAL_NVIC_SetPriority(I2C8_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(I2C8_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(HPDMA3_Channel3_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel3_IRQn);
    /* USER CODE BEGIN I2C8_MspInit 1 */
    /* USER CODE END I2C8_MspInit 1 */
  }

}

void HAL_I2C_MspPostInit(I2C_HandleTypeDef* hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* USER CODE BEGIN I2C_MspPostInit 0 */

	/* USER CODE END I2C_MspPostInit 0 */
	if(I2C8 == hi2c->Instance)
	{
   	    /**I2C8 GPIO Configuration
		   PZ3      ------> I2C8_SDA
		   PZ4      ------> I2C8_SCL
		*/
	    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF8_I2C8;
	    HAL_GPIO_Init(GPIOZ, &GPIO_InitStruct);
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  /* USER CODE BEGIN I2C8_MspDeInit 0 */

  /* USER CODE END I2C8_MspDeInit 0 */
  if(I2C8 == hi2c->Instance)
  {
	/* Peripheral clock disable */
  __HAL_RCC_I2C8_CLK_DISABLE();

    /**I2C8 GPIO Configuration
    PZ3      ------> I2C8_SDA
    PZ4      ------> I2C8_SCL
    */
    HAL_GPIO_DeInit(GPIOZ, GPIO_PIN_3 | GPIO_PIN_4);

    /* I2C8 DMA DeInit */
    HAL_DMA_DeInit(hi2c->hdmatx);
    HAL_DMA_DeInit(hi2c->hdmarx);

    /* I2C8 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2C8_IRQn);
	
	/* USER CODE BEGIN I2C8_MspDeInit 1 */
	/* USER CODE END I2C8_MspDeInit 1 */
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


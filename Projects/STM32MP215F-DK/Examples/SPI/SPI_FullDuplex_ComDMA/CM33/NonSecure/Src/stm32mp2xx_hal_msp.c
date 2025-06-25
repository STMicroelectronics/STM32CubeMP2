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

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  if(SPI1 == hspi->Instance)
  {
    /* USER CODE BEGIN SPI_MspInit 0 */
    /* USER CODE END SPI_MspInit 0 */

    if(IS_DEVELOPER_BOOT_MODE())
    {
       HAL_PWREx_EnableSupply(PWR_PVM_VDDIO3);
    }

    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();

    /* SPI1 DMA Init */
    /* SPI1_TX Init */
    hdma_tx.Instance               = HPDMA3_Channel2;
    hdma_tx.Init.Request           = HPDMA_REQUEST_SPI1_TX;
    hdma_tx.Init.Direction         = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.SrcInc            = DMA_SINC_INCREMENTED;
    hdma_tx.Init.DestInc           = DMA_DINC_FIXED;
    hdma_tx.Init.SrcDataWidth      = DMA_SRC_DATAWIDTH_BYTE;
    hdma_tx.Init.DestDataWidth     = DMA_DEST_DATAWIDTH_BYTE;
    hdma_tx.Init.SrcBurstLength    = 1;
    hdma_tx.Init.DestBurstLength   = 1;
    hdma_tx.Init.Priority          = DMA_HIGH_PRIORITY;
    hdma_tx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    hdma_tx.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_tx);

    /* Configure the DMA Stream */
    if (HAL_OK != HAL_DMA_Init(&hdma_tx))
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi, hdmatx, hdma_tx);

    if(IS_DEVELOPER_BOOT_MODE())
    {
       HAL_DMA_ConfigChannelAttributes(hspi->hdmatx, (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
    }

    /* SPI1_RX Init */
    hdma_rx.Instance               = HPDMA3_Channel3;
    hdma_rx.Init.Request           = HPDMA_REQUEST_SPI1_RX;
    hdma_rx.Init.Direction         = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.SrcInc            = DMA_SINC_FIXED;
    hdma_rx.Init.DestInc           = DMA_DINC_INCREMENTED;
    hdma_rx.Init.SrcDataWidth      = DMA_SRC_DATAWIDTH_BYTE;
    hdma_rx.Init.DestDataWidth     = DMA_DEST_DATAWIDTH_BYTE;
    hdma_rx.Init.SrcBurstLength    = 1;
    hdma_rx.Init.DestBurstLength   = 1;
    hdma_rx.Init.Priority          = DMA_HIGH_PRIORITY;
    hdma_rx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    hdma_rx.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT1 | DMA_DEST_ALLOCATED_PORT0;

    HAL_DMA_DeInit(&hdma_rx);
    if (HAL_OK != HAL_DMA_Init(&hdma_rx))
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi, hdmarx, hdma_rx);

    if(IS_DEVELOPER_BOOT_MODE())
    {
       HAL_DMA_ConfigChannelAttributes(hspi->hdmarx, (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
    }

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(HPDMA3_Channel3_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel3_IRQn);
    /* USER CODE BEGIN SPI_MspInit 1 */
    /* USER CODE END SPI_MspInit 1 */
  }

}

void HAL_SPI_MspPostInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* USER CODE BEGIN SPI_MspPostInit 0 */

	/* USER CODE END SPI_MspPostInit 0 */
	if(SPI1 == hspi->Instance)
	{
	    GPIO_InitStruct.Pin       = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_11;
	    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF2_SPI_I2S1;
	    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin       = GPIO_PIN_5;
	    GPIO_InitStruct.Alternate = GPIO_AF3_SPI_I2S1;
	    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  /* USER CODE BEGIN SPI_MspDeInit 0 */

  /* USER CODE END SPI_MspDeInit 0 */
  if(SPI1 == hspi->Instance)
  {
	/* Peripheral clock disable */
	__HAL_RCC_SPI1_CLK_DISABLE();

	/**SPI1 GPIO Configuration
	   PD1   ------>  SPI1_MISO
	   PI5   ------>  SPI1_MOSI
	   PD11  ------>  SPI1_SCK
	   PD2   ------>  SPI1_NSS
	*/
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_11);
    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_5);

    /* SPI DMA DeInit */
    HAL_DMA_DeInit(hspi->hdmatx);
    HAL_DMA_DeInit(hspi->hdmarx);

    /* SPI interrupt DeInit */
    HAL_NVIC_DisableIRQ(SPI1_IRQn);

	/* USER CODE BEGIN SPI_MspDeInit 1 */
	/* USER CODE END SPI_MspDeInit 1 */
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


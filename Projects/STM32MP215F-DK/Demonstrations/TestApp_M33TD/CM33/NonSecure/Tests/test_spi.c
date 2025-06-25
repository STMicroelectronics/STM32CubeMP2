/**
  ******************************************************************************
  * @file    test_spi.c
  * @author  MCD Application Team
  * @brief   SPI test routines.
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
/**
  * @brief Includes required for SPI test routines.
  */
#include "test_spi.h"
#include "app_freertos.h"

#ifdef ENABLE_SPI_TEST  // Conditionally compile the SPI test code

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFERSIZE                       (COUNTOF(aSpiTxBuffer) - 1)

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

enum {
    TRANSFER_WAIT,
    TRANSFER_COMPLETE,
    TRANSFER_ERROR
};

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

/* Buffer used for transmission */
const uint8_t aSpiTxBuffer[] = "****SPI - Two Boards communication based on DMA **** SPI Message ********* SPI Message *********";

/* Buffer used for reception */
#define BUFFER_ALIGNED_SIZE (((BUFFERSIZE + 31) / 32) * 32)
ALIGN_32BYTES(uint8_t aSpiRxBuffer[BUFFER_ALIGNED_SIZE]);

/* Transfer state */
__IO uint32_t wTransferState = TRANSFER_WAIT;

/* USER CODE BEGIN PV */
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;
/* USER CODE END PV */

/* Exported functions --------------------------------------------------------*/
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
/**
  * @brief  Initialize the SPI peripheral and configure the test environment.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (IS_DEVELOPER_BOOT_MODE())
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

    HAL_DMA_DeInit(&hdma_tx);

    if (HAL_OK != HAL_DMA_Init(&hdma_tx))
    {
        return TEST_FAIL;
    }

    __HAL_LINKDMA(&SpiHandle, hdmatx, hdma_tx);

    if (IS_DEVELOPER_BOOT_MODE())
    {
        HAL_DMA_ConfigChannelAttributes(SpiHandle.hdmatx, (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
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
        return TEST_FAIL;
    }

    __HAL_LINKDMA(&SpiHandle, hdmarx, hdma_rx);

    if (IS_DEVELOPER_BOOT_MODE())
    {
        HAL_DMA_ConfigChannelAttributes(SpiHandle.hdmarx, (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
    }

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);

    HAL_NVIC_SetPriority(HPDMA3_Channel3_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel3_IRQn);

    /* Configure the SPI peripheral */
    SpiHandle.Instance               = SPI1;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.FifoThreshold     = SPI_FIFO_THRESHOLD_01DATA;
    SpiHandle.Init.CRCPolynomial     = 7;
    SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
    SpiHandle.Init.NSS               = SPI_NSS_SOFT;
    SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
    SpiHandle.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
    SpiHandle.Init.Mode              = SPI_MODE_MASTER;

    if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        return TEST_FAIL;
    }

    GPIO_InitStruct.Pin       = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_11;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_SPI_I2S1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = GPIO_PIN_5;
    GPIO_InitStruct.Alternate = GPIO_AF3_SPI_I2S1;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    return TEST_OK;
}

/**
  * @brief  Run the SPI test routine.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_Run(void)
{

		osDelay(3000);

		  /*##-2- Start the Full Duplex Communication process ########################*/
		  /* While the SPI in TransmitReceive process, user can transmit data through
		     "aSpiTxBuffer" buffer & receive data through "aSpiRxBuffer" */

		  if(HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t*)aSpiTxBuffer, (uint8_t *)aSpiRxBuffer, BUFFERSIZE) != HAL_OK)
		  {
		    /* Transfer error in transmission process */
		    return TEST_FAIL;
		  }

		  /*##-3- Wait for the end of the transfer ###################################*/
		  /*  Before starting a new communication transfer, you must wait the callback call
		      to get the transfer complete confirmation or an error detection.
		      For simplicity reasons, this example is just waiting till the end of the
		      transfer, but application may perform other tasks while transfer operation
		      is ongoing. */
		  uint32_t startTime = osKernelGetTickCount(); // Get the current tick count
		  while (wTransferState == TRANSFER_WAIT)
		  {
			if ((osKernelGetTickCount() - startTime) >= 1000) // Timeout: 1 second
			{
			  return TEST_FAIL;
			}
			osDelay(1); // Allow other tasks to run
		  }

		  switch(wTransferState)
		  {
		    case TRANSFER_COMPLETE :
		      /*##-4- Compare the sent and received buffers ##############################*/
		      if(Buffercmp((uint8_t*)aSpiTxBuffer, (uint8_t*)aSpiRxBuffer, BUFFERSIZE))
		      {
		        /* Processing Error */
		        return TEST_FAIL;
		      }
		      break;
		    default :
		      return TEST_FAIL;
		      break;
		  }

    return TEST_OK;
}

/**
  * @brief  De-initialize the SPI peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_DeInit(void)
{
	HAL_SPI_DeInit(&SpiHandle);

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
    HAL_DMA_DeInit(SpiHandle.hdmatx);
    HAL_DMA_DeInit(SpiHandle.hdmarx);

    /* SPI interrupt DeInit */
    HAL_NVIC_DisableIRQ(SPI1_IRQn);



    return TEST_OK;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  wTransferState = TRANSFER_ERROR;
}

/**
  * @brief  Acquire SPI resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_ResourceAcquire(void)
{
    /* Enable GPIOs power supplies */
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(91)))
    {
        if (IS_DEVELOPER_BOOT_MODE())
        {
            __HAL_RCC_GPIOD_CLK_ENABLE();
            __HAL_RCC_GPIOI_CLK_ENABLE();
        }
    }

    /* Acquire SPI using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_SPI1_ID))
    {
        return TEST_FAIL;
    }

    /* Acquire HPDMA3 Channel 2 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2)))
    {
        return TEST_FAIL;
    }

    /* Acquire HPDMA3 Channel 3 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(3)))
    {
        return TEST_FAIL;
    }

    /* Acquire GPIOD1 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(1)))
    {
        return TEST_FAIL;
    }

    /* Acquire GPIOD2 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(2)))
    {
        return TEST_FAIL;
    }

    /* Acquire GPIOD11 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(11)))
    {
        return TEST_FAIL;
    }

    /* Acquire GPIOI5 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOI, RESMGR_GPIO_PIN(5)))
    {
        return TEST_FAIL;
    }

    return TEST_OK;
}

/**
  * @brief  Release SPI resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_ResourceRelease(void)
{
    /* Release HPDMA3 Channel 2 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2)))
    {
        return TEST_FAIL;
    }

    /* Release HPDMA3 Channel 3 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(3)))
    {
        return TEST_FAIL;
    }

    /* Release SPI1 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_SPI1_ID))
    {
        return TEST_FAIL;
    }

    /* Release GPIOD1 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(1)))
    {
        return TEST_FAIL;
    }

    /* Release GPIOD2 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(2)))
    {
        return TEST_FAIL;
    }

    /* Release GPIOD11 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(11)))
    {
        return TEST_FAIL;
    }

    /* Release GPIOI5 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOI, RESMGR_GPIO_PIN(5)))
    {
        return TEST_FAIL;
    }

    return TEST_OK;
}

#endif /* ENABLE_SPI_TEST */  // End of conditional compilation

/* USER CODE END Test_SPI_Implementation */

/**
  ******************************************************************************
  * @file    test_i3c.c
  * @author  MCD Application Team
  * @brief   I3C test routines.
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
  * @brief Includes required for I3C test routines.
  */
#include "test_i3c.h"
#include "app_freertos.h"

#ifdef ENABLE_I3C_TEST  // Conditionally compile the I3C test code

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/** @addtogroup I3C_Test
  * @{
  */

/** @defgroup I3C_Private_Defines Private Defines
  * @brief Private defines for I3C test routines.
  * @{
  */
#define I3C_IDX_FRAME_1         0U  /**< Index of Frame 1 */
#define I3C_IDX_FRAME_2         1U  /**< Index of Frame 2 */

#define DEVICE_STA_ADDR         (0xC4 >> 1) /**< Address must be shifted to the right for 7 bits definition */

/* Size of Transmission buffer */
#define TXBUFFERSIZE            (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE            TXBUFFERSIZE

#define COUNTOF(__BUFFER__)     (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/**
  * @}
  */

/** @defgroup I3C_Private_Variables Private Variables
  * @brief Private variables for I3C test routines.
  * @{
  */
I3C_HandleTypeDef hi3c2;
DMA_HandleTypeDef handle_HPDMA3_Channel2;
DMA_HandleTypeDef handle_HPDMA3_Channel1;
DMA_HandleTypeDef handle_HPDMA3_Channel0;

/* Context buffer related to Frame context, contain different buffer values for a communication */
I3C_XferTypeDef aContextBuffers[2];

/* Number of Targets detected during DAA procedure */
__IO uint32_t uwTargetCount = 0;

/* Buffer used for transmission */
ALIGN_32BYTES(uint8_t aTxBuffer[]) = " ****I2C_TwoBoards communication based on DMA**** ";

/* Buffer used for reception */
ALIGN_32BYTES(uint8_t aRxBuffer[RXBUFFERSIZE]);

/* Buffer used by HAL to compute control data for the Private Communication */
uint32_t aControlBuffer[0xF];

/* Descriptor for private data transmit */
I3C_PrivateTypeDef aPrivateDescriptor[2] = {
    {DEVICE_STA_ADDR, {aTxBuffer, TXBUFFERSIZE}, {NULL, 0}, HAL_I3C_DIRECTION_WRITE},
    {DEVICE_STA_ADDR, {NULL, 0}, {aRxBuffer, RXBUFFERSIZE}, HAL_I3C_DIRECTION_READ}
};
/**
  * @}
  */

/** @defgroup I3C_Private_Functions Private Functions
  * @brief Private functions for I3C test routines.
  * @{
  */

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1 Pointer to the first buffer.
  * @param  pBuffer2 Pointer to the second buffer.
  * @param  BufferLength Length of the buffers to compare.
  * @retval 0 if buffers are identical, >0 if buffers differ.
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
  * @}
  */

/** @defgroup I3C_Exported_Functions Exported Functions
  * @brief Exported functions for I3C test routines.
  * @{
  */

/**
  * @brief  Initialize the I3C peripheral and configure the test environment.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_Init(void)
{

    I3C_FifoConfTypeDef sFifoConfig = {0};
    I3C_CtrlConfTypeDef sCtrlConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};


	    __HAL_RCC_I3C2_CLK_ENABLE();
	    __HAL_RCC_I3C2_FORCE_RESET();
	    __HAL_RCC_I3C2_RELEASE_RESET();

	    /* I3C2 DMA Init */
	    /* HPDMA3_REQUEST_I3C2_TX Init */
	    handle_HPDMA3_Channel1.Instance = HPDMA3_Channel1;
	    handle_HPDMA3_Channel1.Init.Request = HPDMA_REQUEST_I3C2_TX;
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
		  return TEST_FAIL;
	    }

	    __HAL_LINKDMA(&hi3c2, hdmatx, handle_HPDMA3_Channel1);

	    if (IS_DEVELOPER_BOOT_MODE())
	    {
	      HAL_DMA_ConfigChannelAttributes(hi3c2.hdmatx,
	                                      (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | \
	                                       DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
	    }

	    /* HPDMA3_REQUEST_I3C2_RX Init */
	    handle_HPDMA3_Channel2.Instance = HPDMA3_Channel2;
	    handle_HPDMA3_Channel2.Init.Request = HPDMA_REQUEST_I3C2_RX;
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
		  return TEST_FAIL;
	    }
	    __HAL_LINKDMA(&hi3c2, hdmarx, handle_HPDMA3_Channel2);

	    if (IS_DEVELOPER_BOOT_MODE())
	    {
	      HAL_DMA_ConfigChannelAttributes(hi3c2.hdmarx,
	                                      (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | \
	                                       DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
	    }

	    /* HPDMA3_REQUEST_I3C2_TC Init */
	    handle_HPDMA3_Channel0.Instance = HPDMA3_Channel0;
	    handle_HPDMA3_Channel0.Init.Request = HPDMA_REQUEST_I3C2_CMD;
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
		  return TEST_FAIL;
	    }
	    __HAL_LINKDMA(&hi3c2, hdmacr, handle_HPDMA3_Channel0);
	    if (IS_DEVELOPER_BOOT_MODE())
	    {
	      HAL_DMA_ConfigChannelAttributes(hi3c2.hdmacr,
	                                      (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | \
	                                       DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_CID_STATIC_2));
	    }


	    /* I3C2 interrupt Init */
	    HAL_NVIC_SetPriority(I3C2_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(I3C2_IRQn);

	    /* HPDMA3 interrupt Init */
	    HAL_NVIC_SetPriority(HPDMA3_Channel0_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(HPDMA3_Channel0_IRQn);
	    HAL_NVIC_SetPriority(HPDMA3_Channel1_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(HPDMA3_Channel1_IRQn);
	    HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);

	    /* Add a delay to let startup of High level on the Bus */
	    osDelay(1);

	    /* Then set the pull at default value for I3C communication, mean no pull */
	    GPIO_InitStruct.Pin = GPIO_PIN_12;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_0;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	    hi3c2.Instance = I3C2;
	      hi3c2.Mode = HAL_I3C_MODE_CONTROLLER;
	      hi3c2.Init.CtrlBusCharacteristic.SDAHoldTime = HAL_I3C_SDA_HOLD_TIME_1_5;
	      hi3c2.Init.CtrlBusCharacteristic.WaitTime = HAL_I3C_OWN_ACTIVITY_STATE_0;
	      hi3c2.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x09;
	      hi3c2.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x09;
	      hi3c2.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x7c;
	      hi3c2.Init.CtrlBusCharacteristic.SCLI2CHighDuration = 0x7c;
	      hi3c2.Init.CtrlBusCharacteristic.BusFreeDuration = 0x6a;
	      hi3c2.Init.CtrlBusCharacteristic.BusIdleDuration = 0xf8;
	      if (HAL_I3C_Init(&hi3c2) != HAL_OK)
	      {
			return TEST_FAIL;
	      }

	      /** Configure FIFO
	      */
	      sFifoConfig.RxFifoThreshold = HAL_I3C_RXFIFO_THRESHOLD_1_4;
	      sFifoConfig.TxFifoThreshold = HAL_I3C_TXFIFO_THRESHOLD_1_4;
	      sFifoConfig.ControlFifo = HAL_I3C_CONTROLFIFO_DISABLE;
	      sFifoConfig.StatusFifo = HAL_I3C_STATUSFIFO_DISABLE;
	      if (HAL_I3C_SetConfigFifo(&hi3c2, &sFifoConfig) != HAL_OK)
	      {
			return TEST_FAIL;
	      }

	      /** Configure controller
	      */
	      sCtrlConfig.DynamicAddr = 0;
	      sCtrlConfig.StallTime = 0x00;
	      sCtrlConfig.HotJoinAllowed = DISABLE;
	      sCtrlConfig.ACKStallState = DISABLE;
	      sCtrlConfig.CCCStallState = DISABLE;
	      sCtrlConfig.TxStallState = DISABLE;
	      sCtrlConfig.RxStallState = DISABLE;
	      sCtrlConfig.HighKeeperSDA = DISABLE;
	      if (HAL_I3C_Ctrl_Config(&hi3c2, &sCtrlConfig) != HAL_OK)
	      {
			return TEST_FAIL;
	      }


	      /**I3C2 GPIO Configuration
	      PG0    ------> I3C2_SDA
	      PC12   ------> I3C2_SCL
	        */
	      GPIO_InitStruct.Pin = GPIO_PIN_12;
	      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	      GPIO_InitStruct.Pull = GPIO_PULLUP;
	      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	      GPIO_InitStruct.Alternate = GPIO_AF4_I3C2;
	      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	      GPIO_InitStruct.Pin = GPIO_PIN_0;
	      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	      GPIO_InitStruct.Pull = GPIO_PULLUP;
	      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	      GPIO_InitStruct.Alternate = GPIO_AF4_I3C2;
	      HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);


    return TEST_OK;
}

/**
  * @brief  Run the I3C test routine for the OLED display.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_Run(void)
{

    osDelay(3000); // Delay for 3 seconds

    /*##- Prepare context buffers process ##################################*/
    /* Prepare Transmit context buffer with the different parameters */
    aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
    aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size    = 1;
    aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer   = aTxBuffer;
    aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size      = TXBUFFERSIZE;

    /* Prepare Receive context buffer with the different parameters */
    aContextBuffers[I3C_IDX_FRAME_2].CtrlBuf.pBuffer = aControlBuffer;
    aContextBuffers[I3C_IDX_FRAME_2].CtrlBuf.Size    = 1;
    aContextBuffers[I3C_IDX_FRAME_2].RxBuf.pBuffer   = aRxBuffer;
    aContextBuffers[I3C_IDX_FRAME_2].RxBuf.Size      = RXBUFFERSIZE;

	/*##- Add context buffer transmit in Frame context #####################*/
	if (HAL_I3C_AddDescToFrame(&hi3c2,
							   NULL,
							   &aPrivateDescriptor[I3C_IDX_FRAME_1],
							   &aContextBuffers[I3C_IDX_FRAME_1],
							   aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size,
							   I2C_PRIVATE_WITHOUT_ARB_STOP) != HAL_OK)
	{
	  /* Return TEST_FAIL if an error occurs */
	  return TEST_FAIL;
	}

	/*##- Start the transmission process ###################################*/
	/* Transmit private data processus */
	if (HAL_I3C_Ctrl_Transmit_DMA(&hi3c2, &aContextBuffers[I3C_IDX_FRAME_1]) != HAL_OK)
	{
	  /* Return TEST_FAIL if an error occurs */
	  return TEST_FAIL;
	}

    /*##- Wait for the end of the transfer #################################*/
    /*  Before starting a new communication transfer, you need to check the current
    state of the peripheral; if it's busy you need to wait for the end of current
    transfer before starting a new one.
    For simplicity reasons, this example is just waiting till the end of the
    transfer, but application may perform other tasks while transfer operation
    is ongoing. */
	uint32_t startTime = osKernelGetTickCount(); // Get the current tick count
	while (HAL_I3C_GetState(&hi3c2) != HAL_I3C_STATE_READY)
	{
		if ((osKernelGetTickCount() - startTime) >= 1000) // Timeout: 1 second
		{
		   return TEST_FAIL;
		}
		osDelay(1); // Allow other tasks to run
	}

	osDelay(100);


    /*##- Add context buffer receive in Frame context ######################*/
    if (HAL_I3C_AddDescToFrame(&hi3c2,
                               NULL,
                               &aPrivateDescriptor[I3C_IDX_FRAME_2],
                               &aContextBuffers[I3C_IDX_FRAME_2],
                               aContextBuffers[I3C_IDX_FRAME_2].CtrlBuf.Size,
                               I2C_PRIVATE_WITHOUT_ARB_STOP) != HAL_OK)
	{
	  /* Return TEST_FAIL if an error occurs */
	  return TEST_FAIL;
	}

	/*##- Start the reception process ######################################*/
	/* Receive private data processus */
	if (HAL_I3C_Ctrl_Receive_DMA(&hi3c2, &aContextBuffers[I3C_IDX_FRAME_2]) != HAL_OK)
	{
	  /* Return TEST_FAIL if an error occurs */
	  return TEST_FAIL;
	}

    /*##- Wait for the end of the transfer #################################*/
    /*  Before starting a new communication transfer, you need to check the current
    state of the peripheral; if it's busy you need to wait for the end of current
    transfer before starting a new one.
    For simplicity reasons, this example is just waiting till the end of the
    transfer, but application may perform other tasks while transfer operation
    is ongoing. */
	startTime = osKernelGetTickCount(); // Get the current tick count
	while (HAL_I3C_GetState(&hi3c2) != HAL_I3C_STATE_READY)
	{
		if ((osKernelGetTickCount() - startTime) >= 1000) // Timeout: 1 second
		{
		   return TEST_FAIL;
		}
		osDelay(1); // Allow other tasks to run
	}

	/*##- Compare the sent and received buffers ############################*/
	if (Buffercmp((uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, RXBUFFERSIZE))
	{
		/* Processing Error */
		return TEST_FAIL;
	}

    return TEST_OK;
}


/**
  * @brief  De-initialize the I3C peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_DeInit(void)
{

	  /* Deinitialize the I3C peripheral */
	  HAL_I3C_DeInit(&hi3c2);


	    /* Peripheral clock disable */
	    __HAL_RCC_I3C2_CLK_DISABLE();

	    /**I3C2 GPIO Configuration
	    PG0     ------> I3C2_SDA
	    PC12    ------> I3C2_SCL
	      */
	    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);
	    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_0);

	    /* I3C2 DMA DeInit */
	    HAL_DMA_DeInit(hi3c2.hdmatx);
	    HAL_DMA_DeInit(hi3c2.hdmarx);
	    HAL_DMA_DeInit(hi3c2.hdmacr);

	    /* I3C2 interrupt DeInit */
	    HAL_NVIC_DisableIRQ(I3C2_IRQn);


    return TEST_OK;
}


/**
  * @brief  Controller Reception Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
void HAL_I3C_CtrlRxCpltCallback(I3C_HandleTypeDef *hi3c)
{
    /* Toggle LED3: Transfer in Reception process is correct */
    BSP_LED_Toggle(LED3);
}

/**
  * @brief  Controller Transmit Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
void HAL_I3C_CtrlTxCpltCallback(I3C_HandleTypeDef *hi3c)
{
    /* Toggle LED3: Transfer in transmission process is correct */
    BSP_LED_Toggle(LED3);
}

/**
  * @brief  Acquire I3C resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_ResourceAcquire(void)
{
    /* Acquire I3C2 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_I3C2_ID))
    {
        return TEST_FAIL;
    }

    /* Acquire GPIOC12 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(12)))
    {
        return TEST_FAIL;
    }

    /* Acquire GPIOG0 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(0)))
    {
        return TEST_FAIL;
    }

    /* Enable GPIOs power supplies */
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(92)))
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }

    /* HPDMA3 clock enable */
    if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(85)) == RESMGR_STATUS_ACCESS_OK)
    {
        __HAL_RCC_HPDMA3_CLK_ENABLE();
    }

    /* Acquire HPDMA3 Channel 0 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(0)))
    {
        return TEST_FAIL;
    }

    /* Acquire HPDMA3 Channel 1 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(1)))
    {
        return TEST_FAIL;
    }

    /* Acquire HPDMA3 Channel 2 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2)))
    {
        return TEST_FAIL;
    }

    return TEST_OK;
}

/**
  * @brief  Release I3C resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_ResourceRelease(void)
{
    /* Release GPIOC12 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(12));

    /* Release GPIOG0 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(0));

    /* Release HPDMA3 Channel 0 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(0));

    /* Release HPDMA3 Channel 1 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(1));

    /* Release HPDMA3 Channel 2 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2));

    /* Release I3C2 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_I3C2_ID);

    return TEST_OK;
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* ENABLE_I3C_TEST */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

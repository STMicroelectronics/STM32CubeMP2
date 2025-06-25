/**
  ******************************************************************************
  * @file    test_i2c.c
  * @author  MCD Application Team
  * @brief   I2C test routines.
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
#include "test_i2c.h"
#include "app_freertos.h"

#ifdef ENABLE_I2C_TEST  

/* USER CODE BEGIN Test_I2C_Implementation */

/* Private defines -----------------------------------------------------------*/
/**
  * @brief  I2C slave address for the test.
  */
#define TEST_I2C_SLAVE_ADDR     (0x3C << 1) // 7-bit I2C address shifted for HAL
#define BUS_I2Cx_FREQUENCY   	0x1094102CU
/* Private variables ---------------------------------------------------------*/
/**
  * @brief  Handle for the I2C peripheral.
  */
I2C_HandleTypeDef I2cHandle;

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the I2C peripheral and configure the test environment.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_Init(void)
{

    __HAL_RCC_I2C8_CLK_ENABLE();
    __HAL_RCC_I2C8_FORCE_RESET();
    __HAL_RCC_I2C8_RELEASE_RESET();

    /* I2C8 interrupt Init */
    HAL_NVIC_SetPriority(I2C8_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(I2C8_IRQn);


    I2cHandle.Instance = I2C8;
    I2cHandle.Init.Timing = BUS_I2Cx_FREQUENCY;
    I2cHandle.Init.OwnAddress1 = 0;
    I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.OwnAddress2 = 0;
    I2cHandle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&I2cHandle) != HAL_OK) {
      return TEST_FAIL;
    }

    /* Configure Analogue filter */
    if (HAL_I2CEx_ConfigAnalogFilter(&I2cHandle, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
      return TEST_FAIL;
    }

    /* Configure Digital filter */
    if (HAL_I2CEx_ConfigDigitalFilter(&I2cHandle, 0) != HAL_OK) {
      return TEST_FAIL;
    }

    /* I2C Fast mode Plus enable */
    if (HAL_I2CEx_ConfigFastModePlus(&I2cHandle, I2C_FASTMODEPLUS_ENABLE) != HAL_OK) {
      return TEST_FAIL;
    }

    GPIO_InitTypeDef GPIO_InitStruct;
   
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF8_I2C8;
    HAL_GPIO_Init(GPIOZ, &GPIO_InitStruct);
    

    return TEST_OK;
}

/**
  * @brief  Run the I2C test routine.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_Run(void)
{
    // Define a buffer for the test
    #define TEST_I2C_BUFFER_SIZE (128 * 64 / 8)
    static uint8_t Test_I2C_Buffer[TEST_I2C_BUFFER_SIZE];

    // Wait for the screen to boot
    osDelay(100);

    // List of initialization commands for the OLED display
    uint8_t init_commands[] = {
        0xAE, // Display OFF
        0x20, 0x00, // Set Memory Addressing Mode, Horizontal Addressing Mode
        0xB0, // Set Page Start Address for Page Addressing Mode
        0xC8, // Set COM Output Scan Direction
        0x00, // Set low column address
        0x10, // Set high column address
        0x40, // Set start line address
        0x81, 0xFF, // Set contrast control, Maximum contrast
        0xA1, // Set segment re-map
        0xA6, // Set normal display
        0xA8, 0x3F, // Set multiplex ratio, 1/64 duty
        0xA4, // Output follows RAM content
        0xD3, 0x00, // Set display offset, No offset
        0xD5, 0xF0, // Set display clock divide ratio/oscillator frequency
        0xD9, 0x22, // Set pre-charge period
        0xDA, 0x12, // Set com pins hardware configuration
        0xDB, 0x20, // Set vcomh, 0.77xVcc
        0x8D, 0x14, // Set DC-DC enable, Enable charge pump
        0xAF // Display ON
    };

    // Send all initialization commands
    for (size_t i = 0; i < sizeof(init_commands); i++) {
        if (HAL_I2C_Mem_Write(&I2cHandle, TEST_I2C_SLAVE_ADDR, 0x00, 1, &init_commands[i], 1, 1000) != HAL_OK) {
            return TEST_FAIL;
        }
    }

    // Fill white color
    memset(Test_I2C_Buffer, 0xFF, sizeof(Test_I2C_Buffer));

    // Update the screen with the buffer
    for (uint8_t i = 0; i < 64 / 8; i++) {
        uint8_t page_commands[] = {
            (uint8_t)(0xB0 + i), // Set page address
            0x00, // Set low column address
            0x10  // Set high column address
        };

        // Send page commands
        for (size_t j = 0; j < sizeof(page_commands); j++) {
            if (HAL_I2C_Mem_Write(&I2cHandle, TEST_I2C_SLAVE_ADDR, 0x00, 1, &page_commands[j], 1, 1000) != HAL_OK) {
                return TEST_FAIL;
            }
        }

        // Write the buffer data for the current page
        if (HAL_I2C_Mem_Write(&I2cHandle, TEST_I2C_SLAVE_ADDR, 0x40, 1, &Test_I2C_Buffer[128 * i], 128, 1000) != HAL_OK) {
            return TEST_FAIL;
        }
    }

    APP_LOG_INF("TestApp", "White Background Color set to OLED I2C Display");
    osDelay(3000);

    // Fill black color
    memset(Test_I2C_Buffer, 0x00, sizeof(Test_I2C_Buffer));

    // Update the screen with the buffer
    for (uint8_t i = 0; i < 64 / 8; i++) {
        uint8_t page_commands[] = {
            (uint8_t)(0xB0 + i), // Set page address
            0x00, // Set low column address
            0x10  // Set high column address
        };

        // Send page commands
        for (size_t j = 0; j < sizeof(page_commands); j++) {
            if (HAL_I2C_Mem_Write(&I2cHandle, TEST_I2C_SLAVE_ADDR, 0x00, 1, &page_commands[j], 1, 1000) != HAL_OK) {
                return TEST_FAIL;
            }
        }

        // Write the buffer data for the current page
        if (HAL_I2C_Mem_Write(&I2cHandle, TEST_I2C_SLAVE_ADDR, 0x40, 1, &Test_I2C_Buffer[128 * i], 128, 1000) != HAL_OK) {
            return TEST_FAIL;
        }
    }

    return TEST_OK;
}

/**
  * @brief  De-initialize the I2C peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_DeInit(void)
{
    HAL_I2C_DeInit(&I2cHandle);


    /* Peripheral clock disable */
    __HAL_RCC_I2C8_CLK_DISABLE();

    /* I2C8 GPIO Configuration */
    HAL_GPIO_DeInit(GPIOZ, GPIO_PIN_3 | GPIO_PIN_4);

    /* I2C8 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2C8_IRQn);

    /* DMA interrupt DeInit */
    HAL_NVIC_DisableIRQ(HPDMA3_Channel2_IRQn);
    HAL_NVIC_DisableIRQ(HPDMA3_Channel3_IRQn);


    return TEST_OK;
}

/**
  * @brief  Acquire I2C resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_ResourceAcquire(void)
{
    /* Acquire I2C8 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C8_ID)) {
      return TEST_FAIL;
    }

    /* I2C8 GPIO Configuration */
    /* Acquire GPIOZ3 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOZ, RESMGR_GPIO_PIN(3))) {
      return TEST_FAIL;
    }

    /* Acquire GPIOZ4 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOZ, RESMGR_GPIO_PIN(4))) {
      return TEST_FAIL;
    }

    /* Enable GPIOs power supplies */
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(101))) {
        __HAL_RCC_GPIOZ_CLK_ENABLE();
    }

    return TEST_OK;
}

/**
  * @brief  Release I2C resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_ResourceRelease(void)
{
    /* Release HPDMA3 Channel 2 using Resource manager */
    (void) ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2));

    /* Release HPDMA3 Channel 3 using Resource manager */
    (void) ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(3));

    /* Release I2C8 using Resource manager */
    (void) ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C8_ID);

    /* Release GPIOZ3 using Resource manager */
    (void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOZ, RESMGR_GPIO_PIN(3));

    /* Release GPIOZ4 using Resource manager */
    (void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOZ, RESMGR_GPIO_PIN(4));

    return TEST_OK;
}

#endif /* ENABLE_I2C_TEST */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

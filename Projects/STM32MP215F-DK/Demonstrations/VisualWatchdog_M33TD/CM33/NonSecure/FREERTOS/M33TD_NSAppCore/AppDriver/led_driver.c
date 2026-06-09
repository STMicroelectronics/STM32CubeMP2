/**
    ******************************************************************************
    * @file    led_driver.c
    * @author  MCD Application Team
    * @brief   Board-specific Led Driver implementation for LED control.
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
    * @addtogroup LedDriver
    * @{
    */

/* Includes ------------------------------------------------------------------*/
#include "led_driver.h"
#include "main.h"

/* Private function prototypes -----------------------------------------------*/
static void LedDriver_Init(void);
static void LedDriver_DeInit(void);
static void LedDriver_Toggle(LedId_t led_id);
static void LedDriver_On(LedId_t led_id);
static void LedDriver_Off(LedId_t led_id);

/* Exported driver instance --------------------------------------------------*/
/**
    * @brief LED driver instance.
    */
LedDriverTypeDef led_driver =
{
    .init = LedDriver_Init,
    .deinit = LedDriver_DeInit,
    .led_toggle = LedDriver_Toggle,
    .led_on = LedDriver_On,
    .led_off = LedDriver_Off,
};

/* Private functions ---------------------------------------------------------*/
/**
    * @brief  Initialize the LED driver and underlying BSP LED(s).
    */
static void LedDriver_Init(void)
{
    BSP_LED_Init(LED3); /* Add more as needed for your board */
}


/**
    * @brief  De-initialize the LED driver and underlying BSP LED(s).
    */
static void LedDriver_DeInit(void)
{
    BSP_LED_DeInit(LED3); /* Add more as needed for your board */
}


/**
  * @brief  Toggle the state of the specified LED.
    * @param  led_id Identifier of the LED to toggle.
  */
static void LedDriver_Toggle(LedId_t led_id)
{
    switch (led_id)
    {
        case LED_ID_3:
            BSP_LED_Toggle(LED3);
            break;

        /* Add more cases for other LEDs if needed */
        default:
            break;
    }
}


/**
  * @brief  Turn on the specified LED.
    * @param  led_id Identifier of the LED to turn on.
  */
static void LedDriver_On(LedId_t led_id)
{
    switch (led_id)
    {
        case LED_ID_3:
            BSP_LED_On(LED3);
            break;

        /* Add more cases for other LEDs if needed */
        default:
            break;
    }
}


/**
  * @brief  Turn off the specified LED.
    * @param  led_id Identifier of the LED to turn off.
  */
static void LedDriver_Off(LedId_t led_id)
{
    switch (led_id)
    {
        case LED_ID_3:
            BSP_LED_Off(LED3);
            break;

        /* Add more cases for other LEDs if needed */
        default:
            break;
    }
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

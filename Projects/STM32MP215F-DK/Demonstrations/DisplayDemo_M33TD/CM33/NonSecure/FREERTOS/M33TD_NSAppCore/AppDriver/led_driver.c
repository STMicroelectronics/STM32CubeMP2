/**
  ******************************************************************************
  * @file    led_driver.c
  * @brief   Board-specific LED driver implementation.
  ******************************************************************************
  */

#include "led_driver.h"
#include "main.h"

static void LedDriver_Init(void);
static void LedDriver_DeInit(void);
static void LedDriver_Toggle(LedId_t led_id);
static void LedDriver_On(LedId_t led_id);
static void LedDriver_Off(LedId_t led_id);

LedDriverTypeDef led_driver = {
  .init = LedDriver_Init,
  .deinit = LedDriver_DeInit,
  .led_toggle = LedDriver_Toggle,
  .led_on = LedDriver_On,
  .led_off = LedDriver_Off,
};

static void LedDriver_Init(void)
{
  BSP_LED_Init(LED3);
}

static void LedDriver_DeInit(void)
{
  BSP_LED_DeInit(LED3);
}

static void LedDriver_Toggle(LedId_t led_id)
{
  if (led_id == LED_ID_3)
  {
    BSP_LED_Toggle(LED3);
  }
}

static void LedDriver_On(LedId_t led_id)
{
  if (led_id == LED_ID_3)
  {
    BSP_LED_On(LED3);
  }
}

static void LedDriver_Off(LedId_t led_id)
{
  if (led_id == LED_ID_3)
  {
    BSP_LED_Off(LED3);
  }
}
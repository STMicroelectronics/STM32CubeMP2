/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
#include "stm32mp257f_disco.h"
#include "lowpower_wa.h"

/* Exported constants --------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U
/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);


#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

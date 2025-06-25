/**
  ******************************************************************************
  * @file    /NonSecure/Inc/main.h
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
#include "stm32mp215f_disco.h"

/* Size of the key (in bits) */
#define BIT_SIZE_TEST_KEY (128)

/* Size in bytes of a chunk of data to process */
#define BYTE_SIZE_CHUNK (16)

/* Size of the data (in bytes) to be encrypted, it is
 * just randomly chosen size for this example */
#define PLAIN_DATA_SIZE_AEAD (68)

/* Size in bytes of the encryption/decryption buffers */
#define ENC_DEC_BUFFER_SIZE_AEAD \
    (PSA_AEAD_TAG_MAX_SIZE + PLAIN_DATA_SIZE_AEAD)

/* 128-bit test key */
static const uint8_t test_key_128[] = "Dmnst. test KEY";

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
  ******************************************************************************
  * @file    mx25l512_conf_template.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the
  *          MX25L512 QSPI memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX25L512_CONF_H
#define MX25L512_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32xxxx.h"
#include "stm32xxxx_hal.h"

/** @addtogroup BSP
  * @{
  */
   
#define CONF_MX25L512_READ_ENHANCE      0                       /* MMP performance enhance reade enable/disable */

#define CONF_QSPI_ODS                   MX25L512_CR_ODS_15

#define CONF_QSPI_DUMMY_CLOCK               8U

/* Dummy cycles for STR read mode */
#define MX25L512_DUMMY_CLOCK_STR_11x        8U
#define MX25L512_DUMMY_CLOCK_STR_122        4U
#define MX25L512_DUMMY_CLOCK_STR_x44        6U
/* Dummy cycles for DTR read mode */
#define MX25L512_DUMMY_CLOCK_DTR_11x        8U
#define MX25L512_DUMMY_CLOCK_DTR_122        4U
#define MX25L512_DUMMY_CLOCK_DTR_x44        6U
   
#ifdef __cplusplus
}
#endif

#endif /* MX25L512_CONF_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

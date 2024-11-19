/**
  ******************************************************************************
  * @file    stm32mp2xx_clocks.h
  * @author  MCD Application Team
  * @version $VERSION$
  * @date    $DATE$
  * @brief   Clocks definitions file for VALIDx.
  *          This file is to be included from application stm32mp2_hal_conf.h file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2022 STMicroelectronics</center></h2>
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
#ifndef STM32MP2xx_CLOCKS_H
#define STM32MP2xx_CLOCKS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* ######################### Oscillator Values adaptation ################### */
/**
  * @brief Adjust the value of External High Speed oscillator (HSE) used in your application.
  *        This value is used by the RCC HAL module to compute the system frequency
  *        (when HSE is used as system clock source, directly or through the PLL).
  */
#if !defined (HSE_VALUE)
  #define HSE_VALUE             ((uint32_t) 40000000U)  /*!< Value of the External High Speed oscillator in Hz*/
#endif /* HSE_VALUE */

/**
  * @brief In the following line adjust the External High Speed oscillator (HSE) Startup
  *        Timeout value
  */
#if !defined (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT   ((uint32_t)100U)        /*!< Time out for HSE start up, in ms */
#endif /* HSE_STARTUP_TIMEOUT */

/**
  * @brief Internal High Speed oscillator (HSI) value.
  *        This value is used by the RCC HAL module to compute the system frequency
  *        (when HSI is used as system clock source, directly or through the PLL).
  */
#if !defined (HSI_VALUE)
  #define HSI_VALUE             ((uint32_t) 64000000U)  /*!< Value of the Internal High Speed oscillator in Hz*/
#endif /* HSI_VALUE */

/**
  * @brief In the following line adjust the Internal High Speed oscillator (HSI) Startup
  *        Timeout value
  */
#if !defined (HSI_STARTUP_TIMEOUT)
  #define HSI_STARTUP_TIMEOUT   ((uint32_t)5000U)       /*!< Time out for HSI start up */
#endif /* HSI_STARTUP_TIMEOUT */


/**
  * @brief Internal Low Speed oscillator (LSI) value.
  */
#if !defined (LSI_VALUE)
  #define LSI_VALUE             ((uint32_t)32000U)      /*!< LSI Typical Value in Hz */
#endif /* LSI_VALUE */                                  /*!< Value of the Internal Low Speed oscillator in Hz
                                                          The real value may vary depending on the variations
                                                          in voltage and temperature. */

/**
  * @brief In the following line adjust the Internal Low Speed oscillator (LSI) Startup
  *        Timeout value
  */
#if !defined (LSI_STARTUP_TIMEOUT)
  #define LSI_STARTUP_TIMEOUT   ((uint32_t)1000U)       /*!< Time out for LSI start up, in ms*/
  #define LSI_STARTUP_TIME   LSI_STARTUP_TIMEOUT        /*!< legacy name used inside iWDG driver*/
#endif /* LSI_STARTUP_TIME */

/**
  * @brief External Low Speed oscillator (LSE) value.
  */
#if !defined (LSE_VALUE)
  #define LSE_VALUE             ((uint32_t)32768U)      /*!< Value of the External Low Speed oscillator in Hz*/
#endif /* LSE_VALUE */

/**
  * @brief Time out for LSE start up value in ms.
  */
#if !defined (LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT   ((uint32_t)5000U)       /*!< Time out for LSE start up, in ms */
#endif /* LSE_STARTUP_TIMEOUT */

/**
  * @brief Internal  oscillator (MSI) default value.
  *        This value is the default MSI value after Reset.
  */
#if !defined (MSI_VALUE)
  #define MSI_VALUE             ((uint32_t)4000000U)    /*!< Value of the Internal oscillator in Hz*/
#endif /* MSI_VALUE */

/**
  * @brief External clock source for I2S peripheral
  *        This value is used by the I2S HAL module to compute the I2S clock source
  *        frequency, this source is inserted directly through I2S_CKIN pad.
  */
#if !defined (EXTERNAL_CLOCK_VALUE)
  #define EXTERNAL_CLOCK_VALUE  ((uint32_t)12288000U)   /*!< Value of the External clock in Hz*/
#endif /* EXTERNAL_CLOCK_VALUE */

#ifdef __cplusplus
}
#endif

#endif /* STM32MP2xx_CLOCKS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

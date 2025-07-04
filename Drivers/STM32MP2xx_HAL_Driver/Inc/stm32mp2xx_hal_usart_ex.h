/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_usart_ex.h
  * @author  MCD Application Team
  * @brief   Header file of USART HAL Extended module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32MP2xx_HAL_USART_EX_H
#define STM32MP2xx_HAL_USART_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal_def.h"

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @addtogroup USARTEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup USARTEx_Exported_Types USARTEx Exported Types
  * @{
  */
/**
  * @brief  USART Autonomous mode parameters
  */
typedef struct
{
  uint32_t AutonomousModeState;  /*!< Specifies the autonomous mode state.This parameter can be a value of
                                      @ref USARTEx_Autonomous_mode.*/

  uint32_t TriggerSelection;     /*!< Specifies which trigger will activate the Transmission automatically.
                                      This parameter can be a value of @ref USARTEx_Autonomous_Trigger_selection */

  uint32_t TriggerPolarity;      /*!< Specifies the autonomous mode trigger signal polarity.
                                      This parameter can be a value of @ref USARTEx_Autonomous_Trigger_Polarity */

  uint32_t DataSize;             /*!< Specifies the transmitted data size in byte */

  uint32_t IdleFrame;            /*!< Specifies whether the IDLE frame transmission is enabled or disabled.
                                      This parameter can be a value of @ref USARTEx_Autonomous_IDLE_FRAME. */

} USART_AutonomousModeConfTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup USARTEx_Exported_Constants USARTEx Exported Constants
  * @{
  */

/** @defgroup USARTEx_Word_Length USARTEx Word Length
  * @{
  */
#define USART_WORDLENGTH_7B                  (USART_CR1_M1)   /*!< 7-bit long USART frame */
#define USART_WORDLENGTH_8B                  (0x00000000U)              /*!< 8-bit long USART frame */
#define USART_WORDLENGTH_9B                  (USART_CR1_M0)   /*!< 9-bit long USART frame */
/**
  * @}
  */

/** @defgroup USARTEx_Slave_Select_management USARTEx Slave Select Management
  * @{
  */
#define USART_NSS_HARD                        0x00000000U          /*!< SPI slave selection depends on NSS input pin              */
#define USART_NSS_SOFT                        USART_CR2_DIS_NSS    /*!< SPI slave is always selected and NSS input pin is ignored */
/**
  * @}
  */


/** @defgroup USARTEx_Slave_Mode USARTEx Synchronous Slave mode enable
  * @brief    USART SLAVE mode
  * @{
  */
#define USART_SLAVEMODE_DISABLE   0x00000000U     /*!< USART SPI Slave Mode Enable  */
#define USART_SLAVEMODE_ENABLE    USART_CR2_SLVEN /*!< USART SPI Slave Mode Disable */
/**
  * @}
  */

/** @defgroup USARTEx_FIFO_mode USARTEx FIFO  mode
  * @brief    USART FIFO  mode
  * @{
  */
#define USART_FIFOMODE_DISABLE        0x00000000U                   /*!< FIFO mode disable */
#define USART_FIFOMODE_ENABLE         USART_CR1_FIFOEN              /*!< FIFO mode enable  */
/**
  * @}
  */

/** @defgroup USARTEx_TXFIFO_threshold_level USARTEx TXFIFO threshold level
  * @brief    USART TXFIFO level
  * @{
  */
#define USART_TXFIFO_THRESHOLD_1_8   0x00000000U                               /*!< TXFIFO reaches 1/8 of its depth */
#define USART_TXFIFO_THRESHOLD_1_4   USART_CR3_TXFTCFG_0                       /*!< TXFIFO reaches 1/4 of its depth */
#define USART_TXFIFO_THRESHOLD_1_2   USART_CR3_TXFTCFG_1                       /*!< TXFIFO reaches 1/2 of its depth */
#define USART_TXFIFO_THRESHOLD_3_4   (USART_CR3_TXFTCFG_0|USART_CR3_TXFTCFG_1) /*!< TXFIFO reaches 3/4 of its depth */
#define USART_TXFIFO_THRESHOLD_7_8   USART_CR3_TXFTCFG_2                       /*!< TXFIFO reaches 7/8 of its depth */
#define USART_TXFIFO_THRESHOLD_8_8   (USART_CR3_TXFTCFG_2|USART_CR3_TXFTCFG_0) /*!< TXFIFO becomes empty            */
/**
  * @}
  */

/** @defgroup USARTEx_RXFIFO_threshold_level USARTEx RXFIFO threshold level
  * @brief    USART RXFIFO level
  * @{
  */
#define USART_RXFIFO_THRESHOLD_1_8   0x00000000U                               /*!< RXFIFO FIFO reaches 1/8 of its depth */
#define USART_RXFIFO_THRESHOLD_1_4   USART_CR3_RXFTCFG_0                       /*!< RXFIFO FIFO reaches 1/4 of its depth */
#define USART_RXFIFO_THRESHOLD_1_2   USART_CR3_RXFTCFG_1                       /*!< RXFIFO FIFO reaches 1/2 of its depth */
#define USART_RXFIFO_THRESHOLD_3_4   (USART_CR3_RXFTCFG_0|USART_CR3_RXFTCFG_1) /*!< RXFIFO FIFO reaches 3/4 of its depth */
#define USART_RXFIFO_THRESHOLD_7_8   USART_CR3_RXFTCFG_2                       /*!< RXFIFO FIFO reaches 7/8 of its depth */
#define USART_RXFIFO_THRESHOLD_8_8   (USART_CR3_RXFTCFG_2|USART_CR3_RXFTCFG_0) /*!< RXFIFO FIFO becomes full             */
/**
  * @}
  */

/** @defgroup USARTEx_Autonomous_mode USARTEx Autonomous Mode
  * @brief    USART Autonomous mode
  * @{
  */
#define USART_AUTONOMOUS_MODE_DISABLE  0x00000000U          /*!< Autonomous mode disable */
#define USART_AUTONOMOUS_MODE_ENABLE   USART_AUTOCR_TRIGEN  /*!< Autonomous mode enable  */
/**
  * @}
  */

/** @defgroup USARTEx_Autonomous_Trigger_Polarity USARTEx Autonomous Trigger Polarity
  * @brief    USART Trigger polarity edge selection
  * @{
  */
#define USART_TRIG_POLARITY_RISING   0x00000000U           /*!< USART triggered on rising edge  */
#define USART_TRIG_POLARITY_FALLING  USART_AUTOCR_TRIGPOL  /*!< USART triggered on falling edge */
/**
  * @}
  */

/** @defgroup USARTEx_Autonomous_IDLE_FRAME USARTEx Autonomous IDLE Frame
  * @brief    USART IDLE frame transmission
  * @{
  */
#define USART_IDLE_FRAME_ENABLE      0x00000000U          /*!< IDLE Frame sent after enabling the transmitter */
#define USART_IDLE_FRAME_DISABLE     USART_AUTOCR_IDLEDIS /*!< IDLE Frame not sent after enabling the transmitter */
/**
  * @}
  */

/*  Only LPUART has triggers on MP25 and MP23; USARTs do not have triggers. */

#if defined(STM32MP21xxxx)

/** @defgroup USARTEx_Autonomous_Trigger_selection USARTEx Autonomous trigger selection
  * @brief    USART Autonomous Trigger selection
  * @{
  */
#define USART_EXTI2_LINE4_TRG       4U   /*!< USART EXTI2 line 4 Internal Trigger     */
#define USART_EXTI2_LINE5_TRG       5U   /*!< USART EXTI2 line 5 Internal Trigger     */
/*!< No Trigger index 6 on USART            */
#define USART_LPTIM3_CH1_TRG        7U   /*!< USART LPTIM3 channel 1 Internal Trigger */
#define USART_LPTIM4_CH1_TRG        8U   /*!< USART LPTIM4 channel 1 Internal Trigger */
#define USART_LPTIM5_OUT_TRG        9U   /*!< USART LPTIM5 out Internal Trigger       */
#define USART_RTC_ALRA_TRG          10U  /*!< USART RTC alarm Internal Trigger        */
#define USART_RTC_WUT_TRG           11U  /*!< USART RTC wakeup Internal Trigger       */
#define USART_EXTI1_LINE6_TRG       12U  /*!< USART EXTI1 line 6 Internal Trigger     */
#define USART_EXTI1_LINE7_TRG       13U  /*!< USART EXTI1 line 7 Internal Trigger     */
#define USART_LPTIM1_CH1_TRG        14U  /*!< USART LPTIM1 channel 1 Internal Trigger */
#define USART_LPTIM2_CH1_TRG        15U  /*!< USART LPTIM2 channel 1 Internal Trigger */
/**
  * @}
  */

#endif /* STM32MP21xxxx */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup USARTEx_Private_Macros USARTEx Private Macros
  * @{
  */

/** @brief  Compute the USART mask to apply to retrieve the received data
  *         according to the word length and to the parity bits activation.
  * @note   If PCE = 1, the parity bit is not included in the data extracted
  *         by the reception API().
  *         This masking operation is not carried out in the case of
  *         DMA transfers.
  * @param  __HANDLE__ specifies the USART Handle.
  * @retval None, the mask to apply to USART RDR register is stored in (__HANDLE__)->Mask field.
  */
#define USART_MASK_COMPUTATION(__HANDLE__)                            \
  do {                                                                \
    if ((__HANDLE__)->Init.WordLength == USART_WORDLENGTH_9B)         \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == USART_PARITY_NONE)             \
      {                                                               \
        (__HANDLE__)->Mask = 0x01FFU;                                 \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x00FFU;                                 \
      }                                                               \
    }                                                                 \
    else if ((__HANDLE__)->Init.WordLength == USART_WORDLENGTH_8B)    \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == USART_PARITY_NONE)             \
      {                                                               \
        (__HANDLE__)->Mask = 0x00FFU;                                 \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x007FU;                                 \
      }                                                               \
    }                                                                 \
    else if ((__HANDLE__)->Init.WordLength == USART_WORDLENGTH_7B)    \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == USART_PARITY_NONE)             \
      {                                                               \
        (__HANDLE__)->Mask = 0x007FU;                                 \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x003FU;                                 \
      }                                                               \
    }                                                                 \
    else                                                              \
    {                                                                 \
      (__HANDLE__)->Mask = 0x0000U;                                   \
    }                                                                 \
  } while(0U)

/**
  * @brief Ensure that USART frame length is valid.
  * @param __LENGTH__ USART frame length.
  * @retval SET (__LENGTH__ is valid) or RESET (__LENGTH__ is invalid)
  */
#define IS_USART_WORD_LENGTH(__LENGTH__) (((__LENGTH__) == USART_WORDLENGTH_7B) || \
                                          ((__LENGTH__) == USART_WORDLENGTH_8B) || \
                                          ((__LENGTH__) == USART_WORDLENGTH_9B))

/**
  * @brief Ensure that USART Negative Slave Select (NSS) pin management is valid.
  * @param __NSS__ USART Negative Slave Select pin management.
  * @retval SET (__NSS__ is valid) or RESET (__NSS__ is invalid)
  */
#define IS_USART_NSS(__NSS__) (((__NSS__) == USART_NSS_HARD) || \
                               ((__NSS__) == USART_NSS_SOFT))

/**
  * @brief Ensure that USART Slave Mode is valid.
  * @param __STATE__ USART Slave Mode.
  * @retval SET (__STATE__ is valid) or RESET (__STATE__ is invalid)
  */
#define IS_USART_SLAVEMODE(__STATE__)   (((__STATE__) == USART_SLAVEMODE_DISABLE ) || \
                                         ((__STATE__) == USART_SLAVEMODE_ENABLE))

/**
  * @brief Ensure that USART FIFO mode is valid.
  * @param __STATE__ USART FIFO mode.
  * @retval SET (__STATE__ is valid) or RESET (__STATE__ is invalid)
  */
#define IS_USART_FIFO_MODE_STATE(__STATE__) (((__STATE__) == USART_FIFOMODE_DISABLE ) || \
                                             ((__STATE__) == USART_FIFOMODE_ENABLE))

/**
  * @brief Ensure that USART TXFIFO threshold level is valid.
  * @param __THRESHOLD__ USART TXFIFO threshold level.
  * @retval SET (__THRESHOLD__ is valid) or RESET (__THRESHOLD__ is invalid)
  */
#define IS_USART_TXFIFO_THRESHOLD(__THRESHOLD__)  (((__THRESHOLD__) == USART_TXFIFO_THRESHOLD_1_8)  || \
                                                   ((__THRESHOLD__) == USART_TXFIFO_THRESHOLD_1_4)  || \
                                                   ((__THRESHOLD__) == USART_TXFIFO_THRESHOLD_1_2)  || \
                                                   ((__THRESHOLD__) == USART_TXFIFO_THRESHOLD_3_4)  || \
                                                   ((__THRESHOLD__) == USART_TXFIFO_THRESHOLD_7_8)  || \
                                                   ((__THRESHOLD__) == USART_TXFIFO_THRESHOLD_8_8))

/**
  * @brief Ensure that USART RXFIFO threshold level is valid.
  * @param __THRESHOLD__ USART RXFIFO threshold level.
  * @retval SET (__THRESHOLD__ is valid) or RESET (__THRESHOLD__ is invalid)
  */
#define IS_USART_RXFIFO_THRESHOLD(__THRESHOLD__)  (((__THRESHOLD__) == USART_RXFIFO_THRESHOLD_1_8)  || \
                                                   ((__THRESHOLD__) == USART_RXFIFO_THRESHOLD_1_4)  || \
                                                   ((__THRESHOLD__) == USART_RXFIFO_THRESHOLD_1_2)  || \
                                                   ((__THRESHOLD__) == USART_RXFIFO_THRESHOLD_3_4)  || \
                                                   ((__THRESHOLD__) == USART_RXFIFO_THRESHOLD_7_8)  || \
                                                   ((__THRESHOLD__) == USART_RXFIFO_THRESHOLD_8_8))
/**
  * @brief Ensure that USART Trigger polarity state is valid.
  * @param __POLARITY__ USART Trigger polarity.
  * @retval SET (__POLARITY__ is valid) or RESET (__POLARITY__ is invalid)
  */
#define IS_USART_TRIGGER_POLARITY(__POLARITY__)  (((__POLARITY__) == USART_TRIG_POLARITY_RISING) ||\
                                                  ((__POLARITY__) == USART_TRIG_POLARITY_FALLING))

/**
  * @brief Ensure that USART IDLE Frame Transmit state is valid.
  * @param __IDLE__ USART IDLE Frame Transmit state.
  * @retval SET (__IDLE__ is valid) or RESET (__IDLE__ is invalid)
  */
#define IS_USART_IDLE_FRAME_TRANSMIT(__IDLE__)  (((__IDLE__) == USART_IDLE_FRAME_ENABLE) ||\
                                                 ((__IDLE__) == USART_IDLE_FRAME_DISABLE))

/**
  * @brief Ensure that USART Trigger source selection is valid.
  * @param __SOURCE__ USART Trigger source selection.
  * @retval SET (__SOURCE__ is valid) or RESET (__SOURCE__ is invalid)
  */
#if defined(STM32MP21xxxx)
#define IS_USART_TRIGGER_SELECTION(__SOURCE__)    ( ((__SOURCE__) >= 4U) &&\
                                                    ((__SOURCE__) <= 15U) && ((__SOURCE__) != 6U) )
#else
/* No trigger for USART on MP25 and MP23 */
#define IS_USART_TRIGGER_SELECTION(__SOURCE__)    ((__SOURCE__) <  0U)
#endif /* STM32MP21xxxx */

/**
  * @brief Ensure that the number of transferred data is valid.
  * @param __SOURCE__ USART TX data size.
  * @retval SET (__SOURCE__ is valid) or RESET (__SOURCE__ is invalid)
  */
#define IS_USART_TX_DATA_SIZE(__SOURCE__)  ((__SOURCE__) <= 0xFFFFU)

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup USARTEx_Exported_Functions
  * @{
  */

/** @addtogroup USARTEx_Exported_Functions_Group1
  * @{
  */

/* IO operation functions *****************************************************/
void HAL_USARTEx_RxFifoFullCallback(USART_HandleTypeDef *husart);
void HAL_USARTEx_TxFifoEmptyCallback(USART_HandleTypeDef *husart);

/**
  * @}
  */

/** @addtogroup USARTEx_Exported_Functions_Group2
  * @{
  */

/* Peripheral Control functions ***********************************************/
HAL_StatusTypeDef HAL_USARTEx_EnableSlaveMode(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USARTEx_DisableSlaveMode(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USARTEx_ConfigNSS(USART_HandleTypeDef *husart, uint32_t NSSConfig);
HAL_StatusTypeDef HAL_USARTEx_EnableFifoMode(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USARTEx_DisableFifoMode(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USARTEx_SetTxFifoThreshold(USART_HandleTypeDef *husart, uint32_t Threshold);
HAL_StatusTypeDef HAL_USARTEx_SetRxFifoThreshold(USART_HandleTypeDef *husart, uint32_t Threshold);
HAL_StatusTypeDef HAL_USARTEx_SetConfigAutonomousMode(USART_HandleTypeDef *husart,
                                                      const USART_AutonomousModeConfTypeDef *sConfig);
HAL_StatusTypeDef HAL_USARTEx_GetConfigAutonomousMode(const USART_HandleTypeDef *husart,
                                                      USART_AutonomousModeConfTypeDef *sConfig);
HAL_StatusTypeDef HAL_USARTEx_ClearConfigAutonomousMode(USART_HandleTypeDef *husart);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32MP2xx_HAL_USART_EX_H */


/**
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

#ifndef STM32MP2xx_HAL_DDR_DDRPHY_FIRMWARE_DDR_PMU_TRAIN_H
#define STM32MP2xx_HAL_DDR_DDRPHY_FIRMWARE_DDR_PMU_TRAIN_H

#include <stdint.h>

#if STM32MP_DDR3_TYPE
#define DDR_PMU_TRAIN_BIN_LEN  21544
#elif STM32MP_DDR4_TYPE
#define DDR_PMU_TRAIN_BIN_LEN  29988
#elif STM32MP_LPDDR4_TYPE
#define DDR_PMU_TRAIN_BIN_LEN  34384
#endif /* STM32MP_LPDDR4_TYPE */

extern uint8_t ddr_pmu_train_bin[DDR_PMU_TRAIN_BIN_LEN];

#endif /* STM32MP2xx_HAL_DDR_DDRPHY_FIRMWARE_DDR_PMU_TRAIN_H */

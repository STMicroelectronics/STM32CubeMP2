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

#include <stdlib.h>

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit.h"
#ifdef __AARCH64__
#include "stm32mp2xx_hal_ddr_ddrphy_firmware_ddr_pmu_train.h"
#endif /* __AARCH64__ */

/*
 * This function loads the training firmware DMEM image and write the
 * Message Block parameters for the training firmware into the SRAM.
 *
 * This function performs the following tasks:
 *
 * -# Load the firmware DMEM segment to initialize the data structures from the
 * DMEM incv file provided in the training firmware package.
 * -# Write the Firmware Message Block with the required contents detailing the training parameters.
 *
 * \return 0 on success.
 */
int32_t ddrphy_phyinit_f_loaddmem(int32_t pstate)
{
  const pmu_smb_ddr_1d_t *msgblkptr;
  int32_t sizeofmsgblk;
  uint16_t *ptr16;
  uint32_t *ptr32;

  VERBOSE("%s Start (pstate=%d)\n", __func__, pstate);

  /* Set a pointer to the message block */
  msgblkptr = &mb_ddr_1d[pstate];

  /* Some basic checks on MessageBlock */
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
  if ((msgblkptr->enableddqs > (8U * (uint8_t)userinputbasic.numactivedbytedfi0)) ||
      (msgblkptr->enableddqs <= 0U))
  {
    ERROR("%s enableddqs is Zero or greater than NumActiveDbytes for Dfi0\n",__func__);
    return -1;
  }
#elif STM32MP_LPDDR4_TYPE
  if ((msgblkptr->enableddqscha % 16U) != 0U || (msgblkptr->enableddqschb % 16U != 0U))
  {
    ERROR("%s Lp3/Lp4 - Number of Dq's Enabled per Channel much be multipe of 16\n",
          __func__);
    return -1;
  }

  if (((int32_t)msgblkptr->enableddqscha > (8U * (uint8_t)userinputbasic.numactivedbytedfi0)) ||
      ((int32_t)msgblkptr->enableddqschb > (8U * (uint8_t)userinputbasic.numactivedbytedfi1)) ||
      ((msgblkptr->enableddqscha == 0U) && (msgblkptr->enableddqschb == 0U)))
  {
    ERROR("%s EnabledDqsChA/B are not set correctly./1\n", __func__);
    return -1;
  }
#endif /* STM32MP_LPDDR4_TYPE */

  sizeofmsgblk = (int32_t)sizeof(mb_ddr_1d[pstate]);

  ptr16 = (uint16_t *)((uintptr_t)&mb_ddr_1d[pstate]);
  ddrphy_phyinit_writeoutmsgblk(ptr16, DMEM_ST_ADDR, sizeofmsgblk);

#ifdef __AARCH64__
  ptr32 = (uint32_t *)(ddr_pmu_train_bin + STM32MP_DDR_FW_DMEM_OFFSET);
#else /* __AARCH64__ */
  ptr32 = (uint32_t *)(STM32MP_DDR_FW_BASE + STM32MP_DDR_FW_DMEM_OFFSET);
#endif /* __AARCH64__ */
  ddrphy_phyinit_writeoutmem(ptr32, DMEM_ST_ADDR + DMEM_BIN_OFFSET,
                             DMEM_SIZE - (int32_t)STM32MP_DDR_FW_DMEM_OFFSET);

  VERBOSE("%s End\n", __func__);

  return 0;
}

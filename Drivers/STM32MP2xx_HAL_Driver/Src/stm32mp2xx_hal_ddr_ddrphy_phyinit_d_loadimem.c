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
 * This function loads the training firmware IMEM image into the SRAM.
 *
 * This function reads the incv files form the firmware package to generate a
 * set of apb writes to load IMEM image into the SRAM. The exact steps in this
 * function are as follows:
 *
 * -# Ensure DRAM is in reset.
 * -# Load the microcontroller memory with the provided training firmware
 * -# Initialize the firmware mailbox structures to be able to communicate with
 * the firmware (see "Mailbox facility for firmware" in the "Cores
 * DDR PHY Training Application Note".
 *
 * \return void
 */
void ddrphy_phyinit_d_loadimem(void)
{
  uint16_t memresetl;
  uint32_t *ptr32;

  VERBOSE("%s Start\n", __func__);

  /*
   * Set memresetl to avoid glitch on BP_MemReset_L during training
   */
  memresetl = CSR_PROTECTMEMRESET_MASK;
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TMASTER | CSR_MEMRESETL_ADDR))), memresetl);

#ifdef __AARCH64__
  ptr32 = (uint32_t *)(ddr_pmu_train_bin + STM32MP_DDR_FW_IMEM_OFFSET);
#else /* __AARCH64__ */
  ptr32 = (uint32_t *)(STM32MP_DDR_FW_BASE + STM32MP_DDR_FW_IMEM_OFFSET);
#endif /* __AARCH64__ */
  ddrphy_phyinit_writeoutmem(ptr32, IMEM_ST_ADDR, IMEM_SIZE);

  VERBOSE("%s End\n", __func__);
}

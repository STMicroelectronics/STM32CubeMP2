/**
  @page DMA_RepeatedBlock Example Description

  @verbatim
  ******************** (C) COPYRIGHT 2023 STMicroelectronics *******************
  * @file    DMA/DMA_RepeatedBlock/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the DMA Repeated Block example
  ******************************************************************************
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This example describes how to configure and use the DMA HAL API to perform repeated
block transactions.

The start of DMA transfer is triggered by software.
    -   Incremented gather DMA transaction : the DMA loads from the source a scattered data buffer, then
    gather data before sending them to the destination in incrementing mode.
    -   Incremented scatter DMA transaction : the DMA loads from the source a gathered data buffer, then
    scatter data before sending them to the destination in incrementing mode.
    -   Decremented gather DMA transaction : the DMA loads from the source a dispersed data buffer, then
    collect data before sending them to the destination in decrementing mode.
    -   Decremented scatter DMA transaction : the DMA loads from the source a centralized data buffer, then
    scatter data before sending them to the destination in decrementing mode.

-   LED used to monitor the transfers status:
    -   **LED3 toggles** when no error detected.
    -   **LED3 is ON** when any error occurred.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents

  - DMA/DMA_RepeatedBlock/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - DMA/DMA_RepeatedBlock/CM33/NonSecure/Inc/stm32mp215f_disco_conf.h  Disco configuration file
  - DMA/DMA_RepeatedBlock/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - DMA/DMA_RepeatedBlock/CM33/NonSecure/Inc/main.h                   Header for main.c module
  - DMA/DMA_RepeatedBlock/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - DMA/DMA_RepeatedBlock/CM33/NonSecure/Src/main.c                   Main program
  - DMA/DMA_RepeatedBlock/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP module
  - DMA/DMA_RepeatedBlock/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file


@par Hardware and Software environment

  - This example runs on STM32MP2xx devices.

  - This example has been tested with STMicroelectronics STM32MP215F-DK
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?
 
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/DMA_RepeatedBlock/lib/firmware'
- scp the firmware DMA_RepeatedBlock_CM33_NonSecure.elf to root@192.168.7.1:/home/root/DMA_RepeatedBlock/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/DMA_RepeatedBlock/
- To run the example on target: 
	On target shell run:
	- cd /home/root/DMA_RepeatedBlock
	- ./fw_cortex_m33.sh start

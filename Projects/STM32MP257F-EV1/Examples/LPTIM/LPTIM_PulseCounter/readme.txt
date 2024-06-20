/**
  @page LPTIM_PulseCounter Low power timer pulse counter example

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    LPTIM/LPTIM_PulseCounter/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LPTIM Pulse counter example
  ******************************************************************************
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This example describes how to configure and use LPTIM to count pulses through
the LPTIM HAL API.

To reduce power consumption, MCU enters stop mode after starting counting. Each
time the counter reaches the maximum value (Period/Autoreload), an interruption
is generated, the MCU is woke up from stop mode and LED3 toggles to indicate
the last state.

In this example Period value is set to 1000, so each time the counter counts
(1000 + 1) rising edges on LPTIM Input pin PZ3, an interrupt is generated and LED3
toggles.

In this example the internal clock provided to the LPTIM4 is LSE (32 Khz),
so the external input is sampled with LSE clock. In order not to miss any event,
the frequency of the changes on the external Input1 signal should never exceed the
frequency of the internal clock provided to the LPTIM4 (LSE for the
present example).

@note This example can not be used in DEBUG mode, this is due to the fact
      that the Cortex-M4 core is no longer clocked during low power mode
      so debugging features are disabled.

-   LED used to monitor the status:
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

  - LPTIM/LPTIM_PulseCounter/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - LPTIM/LPTIM_PulseCounter/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_PulseCounter/CM33/NonSecure/Inc/main.h                   Header for main.c module
  - LPTIM/LPTIM_PulseCounter/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - LPTIM/LPTIM_PulseCounter/CM33/NonSecure/Src/main.c                   Main program
  - LPTIM/LPTIM_PulseCounter/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP module
  - LPTIM/LPTIM_PulseCounter/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file


@par Hardware and Software environment

  - This example runs on STM32MP2xx devices.

  - This example has been tested with STMicroelectronics STM32MP257F-EV1
    board and can be easily tailored to any other supported device
    and development board.

  - Generate pulses on PZ3 (pin 3 in CN5 connector). (Connect a square waveform).

@par How to use it ?
 
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/LPTIM_PulseCounter/lib/firmware'
- scp the signed firmware LPTIM_PulseCounter_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/LPTIM_PulseCounter/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/LPTIM_PulseCounter/
- To run the example on target: 
	On target shell run:
	- cd /home/root/LPTIM_PulseCounter
	- ./fw_cortex_m33.sh start

/**
  @page SCMI_Demo SCMI Demo Example

  @verbatim
  ******************************************************************************
  * @file    SCMI/SCMI_Demo/readme.txt
  * @author  MCD Application Team
  * @brief   Description of SCMI application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This example describes how to configure the SCMI interface.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The UART4 is configured to print the logs from CM33 core

STM32 board LED can be used to monitor the execution completion. Led3 is toggled every 1 sec to show execution has completed.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The example needs to ensure that the SysTick time base is always set to 1 millisecond
 to have correct HAL operation.

  This projects is configured for STM32MP21xxxx devices using STM32CubeMP2 HAL and
  running on the STM32MP21xxxx board from STMicroelectronics.

@note Clock configuration is bypassed as this application works with Cortex M33 as Trusted Domain.
      all the clock configuration is done by M33 core. This example demonstrate usage of
      SCMI (System Control and Management Interface).
      In this demo SCMI clock protocol usages are shown.
      - scmi_clock_protocol_version()`:
     - Reads SCMI Clock Protocol version from the SCMI server
   - scmi_clock_gate():
     - Enables/disables specific clocks via gate control
   - scmi_clock_enable() / scmi_clock_disable():
     - Enables/disables clock
   - scmi_clock_get_config():
     - Reads flags, attributes, and configuration of clocks
   - scmi_clock_get_attributes() and scmi_clock_get_rate():
   - get the attributes and clock rates

@par Keywords

SCMI,ipcc

@par Directory contents

  - SCMI/SCMI_Demo/CM33/NonSecure/Inc/stm32mp215f_disco_conf.h BSP configuration file
  - SCMI/SCMI_Demo/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - SCMI/SCMI_Demo/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - SCMI/SCMI_Demo/CM33/NonSecure/Inc/main.h                   Header for main.c module
  - SCMI/SCMI_Demo/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - SCMI/SCMI_Demo/CM33/NonSecure/Src/main.c                   Main program
  - SCMI/SCMI_Demo/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP module
  - SCMI/SCMI_Demo/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.
  - This example has been tested with STM32MP215-DK board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/SCMI_Demo/lib/firmware'
- scp the signed firmware SCMI_Demo_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/SCMI_Demo/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/SCMI_Demo/
- To run the example on target:
  On target shell run:
  - cd /home/root/SCMI_Demo
  - ./fw_cortex_m33.sh start

 */
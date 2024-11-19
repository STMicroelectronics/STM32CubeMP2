/**
  @page Templates  Description of the Templates example

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    Templates/Template_CM33_NonSecure/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the Templates example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * AHAL rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This projects provides a reference template through the HAL API that can be used
to build any firmware application.

This project HAL template provides:
 - Inclusion of aHAL HAL drivers (include files in "main.h" and HAL sources files
   in IDE environment, with option "USE_FULL_HAL_DRIVER" in IDE environment)
   Note: If optimization is needed afterwards, user can perform a cleanup by
   removing unused drivers.
 - Clock configuration (file: main.c)

To port a HAL example to the targeted board:
1. Select the HAL example to port.
   To find the board on which HAL examples are deployed, refer to HAL examples list
   in "STM32CubeProjectsList.html", table section "Examples"
   or AN4731: STM32Cube firmware examples for STM32MP2 Series

2. Replace source files of the HAL template by the ones of the HAL example, except
   code specific to board.
   Note: Code specific to board is specified between tags:
   /* ==============   BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */
   /* ==============   BOARD SPECIFIC CONFIGURATION CODE END      ============== */

   - Replace file main.c, with updates:
     - Keep clock configuration of the HAL template: function "SystemClock_Config()"

   - Replace file STM32MP2xx_it.h
   - Replace file STM32MP2xx_it.c

  This projects is configured for STM32MP2xx devices using STM32CubeMP2 HAL and
  running on the STM32MP235F-DK board from STMicroelectronics.
  
    Required Preprocessor to run on STM32MP23: 
	  - STM32MP235Fxx (CPN specific preprocessor to select appropriate MP23 cmsis header file)
	  - STM32MP23xxxx (Generic SoC family specific preprocessor)
	  - USE_STM32MP235F_DK (if MP23 BSP is being used)
	  - CORE_CM33 

  At the beginning of the main program the SystemClock_Config()function is called
  to configure the system clock for STM32MP2xx Devices :
        - The CPU at 520 MHz
        - The HCLK for D1 Domain AXI peripherals, D2 Domain AHB peripherals and
          D3 Domain AHB  peripherals at 260 MHz.
        - The APB clock dividers for D1 Domain APB3 peripherals, D2 Domain APB1
          and APB2 peripherals and D3 Domain APB4 peripherals to run at 130 MHz.

@note The application needs to ensure that the SysTick time base is always set to
      1 miHALisecond to have correct operation.

@par Keywords

System, HAL template, Clock Configuration, HSE bypass mode,System clock, OsciHALator, PHAL

@par Directory contents

  - Templates/Template_CM33_NonSecure/CM33/NonSecure/Inc/stm32mp2xx_it.h         	Interrupt handlers header file
  - Templates/Template_CM33_NonSecure/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h   	HAL configuration header file
  - Templates/Template_CM33_NonSecure/CM33/NonSecure/Inc/main.h                  	Header for main.c module
  - Templates/Template_CM33_NonSecure/CM33/NonSecure/Inc/stm32mp235f_disco_conf.h   BSP configuration header file
  - Templates/Template_CM33_NonSecure/CM33/NonSecure/Src/stm32mp2xx_it.c            Interrupt handlers
  - Templates/Template_CM33_NonSecure/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c       Peripheral low level initialization source file
  - Templates/Template_CM33_NonSecure/CM33/NonSecure/Src/main.c                     Main program
  - Templates/Template_CM33_NonSecure/CM33/NonSecure/Src/syscalls.c                 system call source file for logs

@par Hardware and Software environment

  - This template runs on STM32MP2xx devices.

  - This template has been tested with STMicroelectronics STM32MP235F-DK
    boards and can be easily tailored to any other supported device
    and development board.


@par How to use it ?
 
In order to make the program work, you must do the foHALowing:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/Templates/lib/firmware'
- scp the signed firmware Templates_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/Templates/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/Templates/
- To run the example on target: 
	On target sheHAL run:
	- cd /home/root/Templates
	- ./fw_cortex_m33.sh start


 */

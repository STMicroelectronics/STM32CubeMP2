/**
  @page FreeRTOS_ThreadCreation FreeRTOS Thread Creation application

  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    FreeRTOS/FreeRTOS_ThreadCreation/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS Thread Creation application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

How to implement thread creation using CMSIS RTOS API.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals and initialize the Flash interface.
The HAL initializes the TIM6 to generate an interrupt each 1ms, it will be used as timebase for the HAL drivers.
This application uses FreeRTOS, the RTOS initializes the systick to generate an interrupt each 1ms.
The systick is then used for FreeRTOS timebase.
The SystemClock_Config() function is used to configure the system clock for STM32MP215FANx Devices :
The CPU at 400MHz.
The HCLK for D1 Domain AXI and AHB3 peripherals , D2 Domain AHB1/AHB2 peripherals and D3 Domain AHB4  peripherals at 200MHz.
The APB clock dividers for D1 Domain APB3 peripherals, D2 Domain APB1 and APB2 peripherals and D3 Domain APB4 peripherals to  run at 100MHz.

This application creates two threads with the same priority, which execute in a periodic cycle of 15 seconds.

In the first 5 seconds, the thread 1 toggles LED3 each 200 ms and the thread 2 toggles LED3 each 500 ms.
In the following 5 seconds, the thread 1 suspends itself and the thread 2 continue toggling LED3.
In the last 5 seconds, the thread 2 resumes execution of thread 1 then suspends itself, the thread 1 toggles the LED3
each 500 ms.

@error Error Behaviour

LED3 turns ON if any error occurs.

@par FreeRTOS usage hints
  - Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds) based on
    variable incremented in HAL time base ISR.
    This implies that if HAL_Delay() is called from a peripheral ISR process, then the HAL time base interrupt must
    have higher priority (numerically lower) than the peripheral interrupt. Otherwise the caller ISR process will
    be blocked. To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.
  - The application needs to ensure that the HAL time base is always set to 1 millisecond to have correct HAL operation.
  - The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the OS resources
    memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.
  - For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications on
    STM32Cube with RTOS".

@par Keywords

RTOS, FreeRTOS, Threading, LED

@par Directory contents
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/main.c                             Main program
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/stm32mp2xx_hal_timebase_tim.c      HAL timebase file
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/stm32mp2xx_it.c                    Interrupt handlers
    - FreeRTOS/FreeRTOS_ThreadCreation/Src/stm32mp2xx_hal_msp.c               MSP Initialization file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/main.h                             Main program header file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/stm32mp2xx_hal_conf.h              HAL Library Configuration file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/stm32mp2xx_it.h                    Interrupt handlers header file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/FreeRTOSConfig.h                   FreeRTOS Configuration file
    - FreeRTOS/FreeRTOS_ThreadCreation/Inc/stm32mp215f_disco_conf.h           BSP configuration file


@par Hardware and Software environment

  - This application runs on STM32MP215FANx devices.

  - This application has been tested with STM32MP215F-DK board and can be
    easily tailored to any other supported device and development board.



@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/FreeRTOS_ThreadCreation/lib/firmware'
- scp the signed firmware FreeRTOS_ThreadCreation_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/FreeRTOS_ThreadCreation/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/FreeRTOS_ThreadCreation/
- To run the example on target:
	On target shell run:
	- cd /home/root/FreeRTOS_ThreadCreation
	- ./fw_cortex_m33.sh start


 */

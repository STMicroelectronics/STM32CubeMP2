/**
  @page FreeRTOS_Semaphore FreeRTOS Semaphore example

  @verbatim
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_Semaphore/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS Semaphore example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

How to use semaphores with CMSIS RTOS API.

This application creates two threads that toggle LEDs through a shared semaphore,
as following:

The first thread which have the higher priority waits for the semaphore to be released.
Once the semaphore is release this thread will run and toggle LED3 once.
The second thread will run then and will release the semaphore every 500ms.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority()
      function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.

@note The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
      OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

@error Error Behaviour

LED3 turns ON if any error occurs.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

@par Keywords

RTOS, FreeRTOS, Threading, Semaphore, Priorities

@par Directory contents
    - FreeRTOS/FreeRTOS_Semaphore/Src/main.c                        Main program
    - FreeRTOS/FreeRTOS_Semaphore/Src/stm32mp2xx_hal_timebase_tim.c HAL timebase file
    - FreeRTOS/FreeRTOS_Semaphore/Src/stm32mp2xx_it.c               Interrupt handlers
    - FreeRTOS/FreeRTOS_Semaphore/Src/stm32mp2xx_hal_msp.c          MSP Initialization file
    - FreeRTOS/FreeRTOS_Semaphore/Src/system_stm32mp2xx.c           STM32MP2xx system clock configuration file
    - FreeRTOS/FreeRTOS_Semaphore/Inc/main.h                        Main program header file
    - FreeRTOS/FreeRTOS_Semaphore/Inc/stm32mp2xx_hal_conf.h         HAL Library Configuration file
    - FreeRTOS/FreeRTOS_Semaphore/Inc/stm32mp2xx_it.h               Interrupt handlers header file
    - FreeRTOS/FreeRTOS_Semaphore/Inc/FreeRTOSConfig.h              FreeRTOS Configuration file
    - FreeRTOS/FreeRTOS_Semaphore/Inc/stm32mp2xx_nucleo_conf.h      BSP configuration file

@par Hardware and Software environment

  - This application runs on STM32MP257FAIx devices.

  - This application has been tested with STM32MP257F-EV1 board and can be
    easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/FreeRTOS_Semaphore/lib/firmware'
- scp the signed firmware FreeRTOS_Semaphore_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/FreeRTOS_Semaphore/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/FreeRTOS_Semaphore/
- To run the example on target: 
	On target shell run:
	- cd /home/root/FreeRTOS_Semaphore
	- ./fw_cortex_m33.sh start

 */
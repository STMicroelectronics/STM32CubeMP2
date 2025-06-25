/**
  @page FreeRTOS_MsgQueue FreeRTOS Queues application

  @verbatim
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_MsgQueue/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS Queues application.
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

How to use message queues with CMSIS RTOS API.

This application creates three threads one receiver and two sender which will send the msg for toggling LED3 to other
two threads, as following:
Two threads acts as a sender and the other as the receiver. The receiver thread will block on queue reads.
The queue only has space for ten item, as soon as the sender posts a message on the queue (every 0.5 second) the
receiver will unblock, preempt the sender, and remove the item. If the message received from thread 1 then turn ON the
LED3 and If the message received from thread 2 then turn OFF the LED3.

STM32MP257F-EV1's LEDs can be used to monitor the application status:
  - LED3 should toggle as soon as the sender posts a message on the queue (every 0.5 second).
  - LED3 should turns ON in case any error occurs.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds) based on
      variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then the HAL time base interrupt must
      have higher priority (numerically lower) than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.

@note The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the OS resources
      memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications on
STM32Cube with RTOS".

@par Keywords

RTOS, FreeRTOS, Threading, Message, Queues

@par Directory contents
    - FreeRTOS/FreeRTOS_MsgQueue/Src/main.c                             Main program
    - FreeRTOS/FreeRTOS_MsgQueue/Src/stm32mpxx_hal_timebase_tim.c       HAL timebase file
    - FreeRTOS/FreeRTOS_MsgQueue/Src/stm32mpxx_it.c                     Interrupt handlers
    - FreeRTOS/FreeRTOS_MsgQueue/Src/stm32mpxx_hal_msp.c                MSP Initialization file
    - FreeRTOS/FreeRTOS_MsgQueue/Inc/main.h                             Main program header file
    - FreeRTOS/FreeRTOS_MsgQueue/Inc/stm32mpxx_hal_conf.h               HAL Library Configuration file
    - FreeRTOS/FreeRTOS_MsgQueue/Inc/stm32mpxx_it.h                     Interrupt handlers header file
    - FreeRTOS/FreeRTOS_MsgQueue/Inc/FreeRTOSConfig.h                   FreeRTOS Configuration file
    - FreeRTOS/FreeRTOS_MsgQueue/Inc/stm32mp257f_eval_conf.h            BSP configuration file

@par Hardware and Software environment

  - This application runs on STM32MP257FAIx devices.

  - This application has been tested with STM32MP257F-EV1 board and can be
    easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/FreeRTOS_MsgQueue/lib/firmware'
- scp the signed firmware FreeRTOS_MsgQueue_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/FreeRTOS_MsgQueue/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/FreeRTOS_MsgQueue/
- To run the example on target:
	On target shell run:
	- cd /home/root/FreeRTOS_MsgQueue
	- ./fw_cortex_m33.sh start

 */

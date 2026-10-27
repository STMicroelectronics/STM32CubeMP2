/**
  @page LwIP_HTTP_Server_Socket_RTOS LwIP HTTP Server Socket Application

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    LwIP/LwIP_HTTP_Server_Socket_RTOS/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LwIP http server Socket API Application.
  ******************************************************************************
  * @attention
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

@par Application Description

This application guides STM32Cube HAL API users to run a http server application
based on Socket API of LwIP TCP/IP stack
The communication is done with a web browser application in a remote PC.

this HTTP server contains two html pages:
  + the first page (home page) is static, it gives information about LwIP stack.
  + the second page is dynamically refreshed (every 1 s), it shows the RTOS
    statistics in runtime

If a DHCP server is available, a dynamic IP address can be allocated by enabling
the DHCP process (#define LWIP_DHCP to 1 in lwipopts.h)

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals and initialize the Flash interface.
The HAL initializes the TIM6 to generate an interrupt each 1ms, it will be used as time base for the HAL drivers.
This application uses FreeRTOS, the RTOS initializes the systick to generate an interrupt each 1ms.
The systick is then used for FreeRTOS time base.
The SystemClock_Config() function is used to configure the system clock for STM32MP257FAIx Devices :

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in HAL time base ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the HAL time base interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.

For more details about this application, refer to UM1713 "STM32Cube interfacing with LwIP and applications"

@par Keywords

Connectivity, LwIP, Ethernet, HTTP Server, Netconn, TCP/IP, FreeRTOS, DHCP

@par Directory contents

  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/Core/Inc/main.h				Main program header file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/Core/Inc/stm32mp2xx_it.h			STM32 interrupt handlers header file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/Core/Src/main.c				Main program
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/Core/Src/stm32mp2xx_it.c			STM32 interrupt handlers
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c		MPU Support Package file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/Core/Src/stm32mp2xx_hal_timbase_tim.c	Tim Base File
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/FREERTOS/FreeRTOSConfig.h			FreeRTOS configuration options
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/FREERTOS/app_freertos.h			header for app_freertos.c file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/FREERTOS/app_freertos.c			FreeRTOS module
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/LWIP/App/app_ethernet.c			Ethernet specific module
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/LWIP/App/app_ethernet.h			header of app_ethernet.c file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/LWIP/Target/ethernetif.h			header for ethernetif.c file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/LWIP/Target/ethernetif.c			Interfacing LwIP to ETH driver
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/CM33/NonSecure/LWIP/Target/lwipopts.h			LwIP stack configuration options


@par Hardware and Software environment

  - This application runs on STM32MP257FAIx devices.

  - This application has been tested with the following environments:
     - STM32MP257F-DK board
     - Http client: Google Chrome (v55)
     - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - STM32MP257F-DK Set-up
    - Connect the DK board to remote PC (through a crossover ethernet cable)
      or to your local network (through a straight ethernet cable)

  - Remote PC Set-up
    - PC must share the same LAN network configuration with the DK board


@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/LwIP_HTTP_Server_Socket_RTOS/lib/firmware'
- scp the signed firmware LwIP_HTTP_Server_Socket_RTOS_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/LwIP_HTTP_Server_Socket_RTOS/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/LwIP_HTTP_Server_Socket_RTOS/
- To run the example on target:
	On target shell run:
	- cd /home/root/LwIP_HTTP_Server_Socket_RTOS
	- ./fw_cortex_m33.sh start

@par Optional single-board loopback self-test

Set LOOPBACK_TEST_ENABLE to 1 in lwipopts.h to exercise the LwIP stack locally
without requiring a second board or PC. The option is disabled by default.
 */

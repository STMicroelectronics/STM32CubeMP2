/**
  @page LwIP_UDP_Echo_Client LwIP UDP Echo Client Application

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    LwIP/LwIP_UDP_Echo_Client/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LwIP UDP Echo Client Application.
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

This application guides STM32Cube HAL API users to run a UDP Echo Client application
based on Raw API of LwIP TCP/IP stack

To run this application, On the remote PC, open a command prompt window.
(In Windows, select Start > All Programs > Accessories > Command Prompt.)
At the command prompt, enter:
    C:\>echotool /p udp /s
where:
    – /p transport layer protocol used for communication (UDP)
    – /s is the actual mode of connection (Server mode)

Each time the user pushes the User Button 2 of the STM32MP257F-DK board, an echo
request is sent to the server

If a DHCP server is available, a dynamic IP address can be allocated by enabling
the DHCP process (#define LWIP_DHCP to 1 in lwipopts.h)

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32MP257F Devices :
The CPU at 400MHz

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

For more details about this application, refer to UM1713 "STM32Cube interfacing with LwIP and applications"

@par Optional single-board loopback self-test

This application also embeds an optional loopback self-test, disabled by default,
that lets the LwIP stack be exercised on this single board without requiring a
second board or PC: set LOOPBACK_TEST_ENABLE to 1 in lwipopts.h to enable it.

@par Keywords

Connectivity, LwIP, Ethernet, TCP/IP, DHCP, UDP echo client

@par Directory contents

  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/Core/Inc/stm32MP257f_disco_conf.h    HAL configuration file
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/Core/Inc/stm32MP2xx_it.h             STM32 interrupt handlers header file
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/Core/Inc/main.h                      Main program header file
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/Core/Inc/udp_echoclient.h            header of udp_echoclient.h file
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/Core/Src/main.c                      Main program
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/Core/Src/udp_echoclient.c            UDP echo client application
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/Core/Src/stm32mp2xx_it.c             STM32 interrupt handlers
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c        MSP Initialization file
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/LWIP/App/Inc/app_ethernet.h          header of app_ethernet.c file
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/LWIP/App/Inc/app_ethernet.c          Ethernet specific module
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/LWIP/Target/Inc/lwipopts.h           LwIP stack configuration options
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/LWIP/Target/Inc/ethernetif.h         header for ethernetif.c file
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/LWIP/Target/Src/ethernetif.c         Interfacing LwIP to ETH driver

@par Hardware and Software environment

  - This application runs on STM32MP257FAIx devices.

  - This application has been tested with the following environments:
     - STM32MP257F-DK board
     - echotool: (http://bansky.net/echotool/) is used as echo server that sends
       back every incoming data.
     - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - STM32MP257F-DK Set-up
    - Connect the DK board to remote PC (through a crossover ethernet cable)
      or to your local network (through a straight ethernet cable)

  - Remote PC Set-up
    - PC must share the same LAN network configuration with the DK board
      the IP address must be the same as destination address value in (main.h)


@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/LwIP_UDP_Echo_Client/lib/firmware'
- scp LwIP_UDP_Echo_Client_CM33_NonSecure_sign.bin root@192.168.7.1:/home/root/LwIP_UDP_Echo_Client/lib/firmware
- scp fw_cortex_m33.sh root@192.168.7.1:/home/root/LwIP_UDP_Echo_Client/
- To run the example on target:
	On target shell run:
	- cd /home/root/LwIP_UDP_Echo_Client
	- ./fw_cortex_m33.sh start


 */

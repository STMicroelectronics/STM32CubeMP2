/**
  @page LwIP_UDP_Echo_Server LwIP UDP Echo Server Application

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    LwIP/LwIP_UDP_Echo_Server/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LwIP UDP Echo Server Application.
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

This application guides STM32Cube HAL API users to run UDP Echo Server application
based on Raw API of LwIP TCP/IP stack

To run this application, On the remote PC, open a command prompt window.
(In Windows, select Start > All Programs > Accessories > Command Prompt.)
At the command prompt, enter:
  C:\>echotool IP_address /p udp /r 7 /l 7 /n 15 /t 2 /d Testing LwIP UDP echo server
where:
    – IP_address is the actual board’s IP address. By default, the following
    static IP address is used: 192.168.0.10
    – /p transport layer protocol used for communication (UDP)
    – /r is the actual remote port on the echo server (echo port)
    - /l is the actual local port for the client (echo port)
    – /n is the number of echo requests (for application, 15)
    – /t is the connection timeout in seconds (for application, 2)
    – /d is the message to be sent for echo

If a DHCP server is available, a dynamic IP address can be allocated by enabling
the DHCP process (#define LWIP_DHCP to 1 in lwipopts.h)

@par Keywords

Connectivity, LwIP, Ethernet, TCP/IP, DHCP, echo server,

@par Directory contents

  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/LWIP/App/Inc/app_ethernet.h          header of app_ethernet.c file
  - LwIP/LwIP_UDP_Echo_Client/CM33/NonSecure/LWIP/Target/Inc/ethernetif.h         header for ethernetif.c file
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/LWIP/Target/Inc/lwipopts.h           LwIP stack configuration options
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/LWIP/App/Inc/app_ethernet.c          Ethernet specific module
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/LWIP/Target/Src/ethernetif.c         Interfacing LwIP to ETH driver
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/Core/Inc/stm32MP257f_disco_conf.h    HAL configuration file
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/Core/Inc/stm32MP2xx_it.h             STM32 interrupt handlers header file
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/Core/Inc/main.h                      Main program header file
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/Core/Src/main.c                      Main program
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/Core/Src/stm32mp2xx_it.c             STM32 interrupt handlers
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c        MSP Initialization file
  - LwIP/LwIP_UDP_Echo_Server/CM33/NonSecure/Core/Src/Src/udp_echoserver.c        UDP echo server application

@par Hardware and Software environment

  - This application runs on STM32MP257FAIx devices.

  - This application has been tested with the following environments:
     - STM32MP257F-DK board
     - echotool: (http://bansky.net/echotool/) is used as echo client that sends
       data to the server and checking whether they came back
     - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - STM32MP257F-DK Set-up
    - Connect the DK board to remote PC (through a crossover ethernet cable)
      or to your local network (through a straight ethernet cable)

  - Remote PC Set-up
    - PC must share the same LAN network configuration with the disco board


@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/LwIP_UDP_Echo_Server/lib/firmware'
- scp LwIP_UDP_Echo_Server_CM33_NonSecure_sign.bin root@192.168.7.1:/home/root/LwIP_UDP_Echo_Server/lib/firmware
- scp fw_cortex_m33.sh root@192.168.7.1:/home/root/LwIP_UDP_Echo_Server/
- To run the example on target:
	On target shell run:
	- cd /home/root/LwIP_UDP_Echo_Server
	- ./fw_cortex_m33.sh start

@par Optional single-board loopback self-test

Set LOOPBACK_TEST_ENABLE to 1 in lwipopts.h to exercise the LwIP stack locally
without requiring a second board or PC. The option is disabled by default.

 */

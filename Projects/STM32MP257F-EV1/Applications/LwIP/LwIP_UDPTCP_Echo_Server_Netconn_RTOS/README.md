/**
  @page LwIP_UDPTCP_Echo_Server_Netconn_RTOS LwIP UDP/TCP Echo Server Netconn Application

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LwIP UDP/TCP Echo Server Netconn API Application
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

This application guides STM32Cube HAL API users to run a UDP/TCP Echo Server application
based on Netconn API of LwIP TCP/IP stack

To run this application, On the remote PC, open a command prompt window.
(In Windows, select Start > All Programs > Accessories > Command Prompt.)
At the command prompt, enter:
  C:\>echotool IP_address /p tcp /r 7 /l 7 /n 15 /t 2 /d Testing LwIP TCP echo server
or
  C:\>echotool IP_address /p udp /r 7 /l 7 /n 15 /t 2 /d Testing LwIP UDP echo server
where:
    – IP_address is the actual board’s IP address. By default, the following
    static IP address is used: 192.168.0.10
    – /p transport layer protocol used for communication (TCP or UDP)
    – /r is the actual remote port on the echo server (echo port)
    - /l is the actual local port for the client (echo port)
    – /n is the number of echo requests (for application, 15)
    – /t is the connection timeout in seconds (for application, 2)
    – /d is the message to be sent for echo

#### Expected success behavior

 - The board IP address is printed on the HyperTerminal

```
       lwip application started..
       STM32 IpAddress: 10.48.1.2
	   TCP Server listening on PORT 6000
```

If a DHCP server is available, a dynamic IP address can be allocated by enabling
the DHCP process (#define LWIP_DHCP to 1 in lwipopts.h)


@par Optional single-board loopback self-test

This application also embeds an optional loopback self-test, disabled by default,
that lets the LwIP stack be exercised on this single board without requiring a
second board or PC: set LOOPBACK_TEST_ENABLE to 1 in lwipopts.h to enable it.

@par Keywords

Connectivity, LwIP, Ethernet, Echo Server, Netconn, TCP/IP, RTOS, DHCP

@par Directory contents

  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/app_ethernet.h          header of app_ethernet.c file
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/ethernetif.h            header for ethernetif.c file
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/stm32mp2xx_hal_conf.h   HAL configuration file
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/stm32mp2xx_it.h         STM32 interrupt handlers header file
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/main.h                  Main program header file
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/lwipopts.h              LwIP stack configuration options
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/FreeRTOSConfig.h        FreeRTOS configuration options
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/tcpecho.h               header for tcpecho.c
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Inc/udpecho.h               header for udpecho.c
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Src/stm32mp2xx_it.c         STM32 interrupt handlers
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Src/app_ethernet.c          Ethernet specific module
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Src/main.c                  Main program
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Src/system_stm32mp2xx.c     STM32 system clock configuration file
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Src/ethernetif.c            Interfacing LwIP to ETH driver
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Src/tcpecho.c               tcp echo server main thread
  - LwIP/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/Src/udpecho.c               udp echo server main thread


@par Hardware and Software environment

  - This application runs on STM32MP257FAIx devices.

  - This application has been tested with the following environments:
     - STM32MP257F-EV1 board
     - echotool: (http://bansky.net/echotool/) is used as echo client that sends
       data to the server and checking whether they came back
     - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - STM32MP257F-EV1 Set-up
    - Connect the eval board to remote PC (through a crossover ethernet cable)
      or to your local network (through a straight ethernet cable)

  - Remote PC Set-up
    - PC must share the same LAN network configuration with the eval board


@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/lib/firmware'
- scp the signed firmware scp LwIP_UDPTCP_Echo_Server_Netconn_RTOS_CM33_NonSecure_sign.bin root@192.168.7.1:/home/root/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/lib/firmware
- scp the script from Firmware/Utilities/scripts/ scp fw_cortex_m33.sh root@192.168.7.1:/home/root/LwIP_UDPTCP_Echo_Server_Netconn_RTOS/
- To run the example on target:
	On target shell run:
	- cd /home/root/LwIP_UDPTCP_Echo_Server_Netconn_RTOS
	- ./fw_cortex_m33.sh start


 */

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

Each time the user pushes the User Button 2 of the STM32MP257_EVAL board, an echo
request is sent to the server

If a DHCP server is available, a dynamic IP address can be allocated by enabling
the DHCP process (#define LWIP_DHCP to 1 in lwipopts.h)

@par Keywords

Connectivity, LwIP, Ethernet, TCP/IP, DHCP, UDP echo client

@par Directory contents

  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/Core/Inc/stm32MP257f_eval_conf.h     HAL configuration file
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/Core/Inc/stm32MP2xx_it.h             STM32 interrupt handlers header file
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/Core/Inc/main.h                      Main program header file
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/Core/Inc/udp_echoclient.h            header of udp_echoclient.h file
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/Core/Src/main.c                      Main program
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/Core/Src/udp_echoclient.c            UDP echo client application
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/Core/Src/stm32mp2xx_it.c             STM32 interrupt handlers
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/Core/Src/stm32mp2xx_hal_msp.c        MSP Initialization file
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/LWIP/App/Inc/app_ethernet.h          header of app_ethernet.c file
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/LWIP/App/Inc/app_ethernet.c          Ethernet specific module
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/LWIP/Target/Inc/lwipopts.h           LwIP stack configuration options
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/LWIP/Target/Inc/ethernetif.h         header for ethernetif.c file
  - LwIP/LwIP_UDP_Echo_Client/<Core>/NonSecure/LWIP/Target/Src/ethernetif.c         Interfacing LwIP to ETH driver

@par Hardware and Software environment

  - This application runs on STM32MP257FAIx devices.

  - This application has been tested with the following environments:
     - STM32MP257F-EV1 board
     - echotool: (http://bansky.net/echotool/) is used as echo server that sends
       back every incoming data.
     - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - STM32MP257F-EV1 Set-up
    - Connect the eval board to remote PC (through a crossover ethernet cable)
      or to your local network (through a straight ethernet cable)

  - Remote PC Set-up
    - PC must share the same LAN network configuration with the eval board
      the IP address must be the same as destination address value in (main.h)


@par How to use it ?
- For A35 TD
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

- For M33 TD
In order to make the program work, you must do the following :
- Open STM32CubeIDE
- Build with config: CA35_ARCH32_BM
- LwIP_UDP_Echo_Client.bin file would be generated containing the application.
- Refer to the Steps present in Projects\STM32MP257F-EV1\Template_StarterApp_M33TD\README.md to compile remaining binaries.
  LwIP_UDP_Echo_Client.bin need  to be signed. While compiling TFM Secure in StarterAppM33TD add the absolute path to LwIP_UDP_Echo_Client.bin using below argument

  -DSTM32_CA35_FW=`<PATH>`/LwIP_UDP_Echo_Client.bin

- LwIP_UDP_Echo_Client.bin signed binary will be generated in `trusted-firmware-m\config_default\bin folder`.
- This binary Should be programmed along with Binaries generated after continuing steps in Template_StarterApp_M33TD project.

 */

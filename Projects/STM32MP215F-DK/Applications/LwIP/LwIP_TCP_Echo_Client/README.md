/**
  @page LwIP_TCP_Echo_Client LwIP TCP Echo Client Application

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    LwIP/LwIP_TCP_Echo_Client/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LwIP TCP Echo Client Application.
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

This application guides STM32Cube HAL API users to run TCP Echo Client application
based on Raw API of LwIP TCP/IP stack

To run this application, On the remote PC, open a command prompt window.
(In Windows, select Start > All Programs > Accessories > Command Prompt.)
At the command prompt, enter:
    C:\>echotool /p tcp /s
where:
    – /p transport layer protocol used for communication (TCP)
    – /s is the actual mode of connection (Server mode)

Each time the user pushes the User Button 2 of the STM32MP257F-EV1 board, an echo
request is sent to the server

If a DHCP server is available, a dynamic IP address can be allocated by enabling
the DHCP process (#define LWIP_DHCP to 1 in lwipopts.h)

@par Keywords

Connectivity, LwIP, Ethernet, TCP/IP, DHCP, TCP echo client

@par Directory contents

  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/Core/Inc/stm32MP257f_eval_conf.h     HAL configuration file
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/Core/Inc/stm32MP2xx_it.h             STM32 interrupt handlers header file
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/Core/Inc/main.h                      Main program header file
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/Core/Src/main.c                      Main program
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/Core/Src/stm32mp2xx_it.c             STM32 interrupt handlers
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c        MSP Initialization file
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/Core/Src/tcp_echoclient.c            TCP echo client application
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/LWIP/App/Inc/app_ethernet.h          header of app_ethernet.c file
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/LWIP/App/Inc/app_ethernet.c          Ethernet specific module
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/LWIP/Target/Inc/ethernetif.h         header for ethernetif.c file
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/LWIP/Target/Inc/lwipopts.h           LwIP stack configuration options
  - LwIP/LwIP_TCP_Echo_Client/CM33/NonSecure/LWIP/Target/Src/ethernetif.c         Interfacing LwIP to ETH driver

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

@par External Device Tree (A35TD) configuration

On STM32MP215F-DK, the Ethernet peripheral (ETH1) and its pins are owned by the
Cortex-A35 (Linux) by default. To let this Cortex-M33 example use Ethernet, the
external device tree (A35TD) of the "m33_examples" variant must be updated to
release ETH1 and its resources to the Cortex-M33 (CID2, non-secure):

  - In stm32mp2/a35-td/linux/stm32mp215f-dk-ca35tdcid-ostl-m33-examples.dts,
    disable the eth1 node on the Linux side:
      &eth1 { status = "disabled"; };

  - In stm32mp2/a35-td/optee/stm32mp215f-dk-ca35tdcid-ostl-m33-examples-rif.dtsi,
    statically assign the following resources to CID2 (Cortex-M33), non-secure:
      - ETH1 peripheral:     STM32MP21_RIFSC_ETH1_ID -> RIF_CID2
      - ETH1 DMA master bus: RIMU_ID(6)              -> RIF_CID2
      - ETH1 RMII GPIO pins  -> RIF_CID2:
          gpiob: PB11, PB12, PB13, PB15
          gpioc: PC1, PC2
          gpiof: PF0, PF1, PF2, PF8
          gpioh: PH11

Rebuild and deploy the updated A35TD device tree before running this example.

@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/LwIP_TCP_Echo_Client/lib/firmware'
- scp LwIP_TCP_Echo_Client_CM33_NonSecure_sign.bin root@192.168.7.1:/home/root/LwIP_TCP_Echo_Client/lib/firmware
- scp fw_cortex_m33.sh root@192.168.7.1:/home/root/LwIP_TCP_Echo_Client/
- To run the example on target:
	On target shell run:
	- cd /home/root/LwIP_TCP_Echo_Client
	- ./fw_cortex_m33.sh start

@par Optional single-board loopback self-test

Set LOOPBACK_TEST_ENABLE to 1 in lwipopts.h to exercise the LwIP stack locally
without requiring a second board or PC. The option is disabled by default.

 */

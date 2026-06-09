/**
  @page SPI_FullDuplex_ComDMA_LinkedList SPI Full Duplex DMA Linked-List example

  @verbatim
  ******************** (C) COPYRIGHT 2026 STMicroelectronics *******************
  * @file    SPI/SPI_FullDuplex_ComDMA_LinkedList/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the SPI Full Duplex DMA Linked-List example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This example shows how to perform a full-duplex SPI transfer between two
STM32MP257F-EV1 boards using SPI3 and HPDMA3 in Linked-List mode.

Each side builds two DMA linked-lists:
 - TX queue: 3 nodes, each node sends one TX buffer to SPI3 TXDR
 - RX queue: 3 nodes, each node stores received data from SPI3 RXDR into one RX buffer

The SPI wiring is showin below:

   ________________________________             _____________________________
  |        ________________________|           |______________________       |
  |       |     SPI3               |           |        SPI3          |      |
  |       |                        |           |                      |      |
  |       |CLK(PB7)    CN5.D23     |___________|(PB7) CLK  CN5.D23    |      |
  |       |                        |           |                      |      |
  |       |MISO(PB10)  CN5.D21     |___________|(PB10) MISO CN5.D21   |      |
  |       |                        |           |                      |      |
  |       |MOSI(PB8)   CN5.D19     |___________|(PB8) MOSI CN5.D19    |      |
  |       |                        |           |                      |      |
  |       |________________________|           |______________________|      |
  |   __                           |           |                             |
  |  |__|                          |           |                             |
  |  USER2                         |           |                             |
  |                             GND|___________|GND                          |
  |                                |           |                             |
  |_______STM32MP2 Master__________|           |________STM32MP2 Slave_______|

@note Use short wires and connect a common ground.

@par Code flow

- HAL_Init() resets peripherals and initializes the HAL time base.
- SystemClock_Config() configures the device clocks.
- HAL_SPI_Init() configures SPI3 and calls HAL_SPI_MspInit() which configures
  the SPI clock, pins, interrupts and links DMA handles.
- The application builds HPDMA3 linked-lists using HAL DMAEx linked-list APIs
  (HAL_DMAEx_List_*).

For SPI master transfers on STM32MP25, the application must also:
 - Program SPI transfer size (TSIZE)
 - Enable SPI DMA requests (RXDMAEN and TXDMAEN)
 - Start the master transfer engine (generate SCK)

This start/arm sequence is performed using STM32 LL SPI APIs (LL_SPI_*),
because HAL_SPI_TransmitReceive_DMA() is designed for a single contiguous buffer
and does not expose a multi-node scatter/gather interface.

@par User interaction

- Master board: waits for USER2 button press, then starts the transfer.
- Slave board: arms the DMA and waits for the master clock.

At transfer completion, TX and RX buffers are compared (Buffercmp) and LED3
indicates the result.

@par LEDs

- LED3 toggles at 0.5s when the transmission/reception is complete.
- LED3 turns ON permanently if an error occurs.

@note It is recommended to set "MasterKeepIOState" to ENABLE in master mode to
      avoid glitches on the SPI lines when the peripheral is disabled.

@note Cache maintenance may be required when DMA buffers reside in cacheable
      memories. Buffers should be aligned to the L1 cache line size (32 bytes)
      and cleaned/invalidated as appropriate.

@par Keywords

Connectivity, SPI, Full-duplex, DMA, Linked-List, Master, Slave, MISO, MOSI

@par Directory contents

  - SPI/SPI_FullDuplex_ComDMA_LinkedList/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h   HAL configuration file
  - SPI/SPI_FullDuplex_ComDMA_LinkedList/CM33/NonSecure/Inc/stm32mp2xx_it.h         Interrupt handlers header file
  - SPI/SPI_FullDuplex_ComDMA_LinkedList/CM33/NonSecure/Inc/main.h                  Header for main.c module
  - SPI/SPI_FullDuplex_ComDMA_LinkedList/CM33/NonSecure/Src/main.c                  Main program
  - SPI/SPI_FullDuplex_ComDMA_LinkedList/CM33/NonSecure/Src/stm32mp2xx_it.c         Interrupt handlers
  - SPI/SPI_FullDuplex_ComDMA_LinkedList/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c    HAL MSP file
  - SPI/SPI_FullDuplex_ComDMA_LinkedList/CM33/NonSecure/Src/system_stm32mp2xx.c     STM32MP2xx system source file

@par Hardware and Software environment

 - This example runs on STM32MP257FAIx devices.
 - It has been tested with STM32MP257F-EV1 and can be adapted to other boards.

@note Reset the slave board first, then reset the master board for best results.

@par How to use it ?

- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
  - Uncomment "#define MASTER_BOARD" and load the project on the Master board
  - Comment "#define MASTER_BOARD" and load the project on the Slave board

Example deployment on target Linux:
- ssh root@192.168.7.1 'mkdir -p /home/root/SPI_FullDuplex_ComDMA_LinkedList/lib/firmware'
- scp the signed firmware SPI_FullDuplex_ComDMA_LinkedList_CM33_NonSecure_sign.bin \
      root@192.168.7.1:/home/root/SPI_FullDuplex_ComDMA_LinkedList/lib/firmware
- scp Firmware/Utilities/scripts/fw_cortex_m33.sh \
      root@192.168.7.1:/home/root/SPI_FullDuplex_ComDMA_LinkedList/

On target shell:
- cd /home/root/SPI_FullDuplex_ComDMA_LinkedList
- ./fw_cortex_m33.sh start

 */
 

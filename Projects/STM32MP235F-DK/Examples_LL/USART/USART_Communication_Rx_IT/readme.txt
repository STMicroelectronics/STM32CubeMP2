/**
  @page USART_Communication_Rx_IT USART Receiver example (IT Mode)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_Communication_Rx_IT/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_Communication_Rx_IT example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

Configuration of GPIO and USART peripherals to receive characters 
from an HyperTerminal (PC) in Asynchronous mode using an interrupt. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size).

At the beginning of the main program the SystemClock_Config() function is called to configure the system clock for STM32MP25xx Devices

USART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
GPIO associated to User push-button is linked with EXTI.
USART RX Not Empty interrupt is enabled.
Virtual Com port feature of STLINK could be used for UART communication between board and PC.

Example execution:
When character is received on USART Rx line, a RXNE interrupt occurs.
USART IRQ Handler routine is then checking received character value.
On a specific value ('S' or 's'), LED3 is turned On.
Received character is echoed on Tx line.
On press on User push-button, LED is turned Off.
In case of errors, LED turns ON continuously.

@par Keywords

Connectivity, USART, Asynchronous mode, RS-232, baud rate, Interrupt, HyperTerminal, Reception

@par Directory contents 

  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Inc/main.h                   Header for main.c module
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL Configuration file
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Src/stm32h7xx_it.c           Interrupt handlers
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Src/main.c                   Main program
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP file
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file


@par Hardware and Software environment

  - This example runs on STM32MP25xx devices.
    
  - This example has been tested with STM32MP257F-DK board and can be
    easily tailored to any other supported device and development board.
    
  - STM32MP257F-DK Set-up
    In order to select use of Virtual Com port feature of STLINK for connection between STM32MP257F-DK and PC
    If so, please ensure that USART communication between the target MPU and ST-LINK MPU is properly enabled 
    on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)

    Connect USART1 TX/RX to respectively RX and TX pins of PC UART (could be done through a USB to UART adapter) :
    - Connect STM32 MPU board USART1 TX pin (Pin 2 of CN5 connector)
      to PC COM port RX signal
    - Connect STM32 MPU board USART1 RX pin (Pin 3 of CN5 connector)
      to PC COM port TX signal
    - Connect STM32 MPU board GND to PC COM port GND signal
    
  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control). 

  - Launch the program. Enter characters on PC communication SW side.

@par How to use it ?
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/USART_Communication_Rx_IT/lib/firmware'
- scp the firmware USART_Communication_Rx_IT_CM33_NonSecure.elf to root@192.168.7.1:/home/root/USART_Communication_Rx_IT/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/USART_Communication_Rx_IT/
- To run the example on target: 
	On target shell run:
	- cd /home/root/USART_Communication_Rx_IT
	- ./fw_cortex_m33.sh start

 */

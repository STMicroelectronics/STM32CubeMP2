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

At the beginning of the main program the SystemClock_Config() function is called to configure the system clock for STM32MP21xxxx Devices

USART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
GPIO associated to User push-button is linked with EXTI.
USART RX Not Empty interrupt is enabled.
UART4 communication can be connected to a USB-to-UART adapter through CN10 or to the ST-LINK Virtual COM port (VCP2), depending on the board configuration.

Example execution:
When a character is received on the USART RX line, an RXNE interrupt occurs and the character is echoed on the TX line.
When the received character is 'S' or 's', LED3 is turned On.
On press on USER2 push-button, LED3 is turned Off (allows to restart the sequence).
In case of errors, LED turns ON continuously.

@par Keywords

Connectivity, USART, Asynchronous mode, RS-232, baud rate, Interrupt, HyperTerminal, Reception

@par Directory contents 

  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Inc/stm32mp2xx_it.h            Interrupt handlers header file
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Inc/main.h                     Header for main.c module
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h      HAL Configuration file
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Inc/stm32mp215f_disco_conf.h   STM32MP215F_DK configuration file
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Src/stm32mp2xx_it.c            Interrupt handlers
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Src/main.c                     Main program
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c       HAL MSP file
  - USART/USART_Communication_Rx_IT/CM33/NonSecure/Src/system_stm32mp2xx.c        STM32MP2xx system source file


@par Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.
    
  - This example has been tested with STM32MP215F-DK board and can be
    easily tailored to any other supported device and development board.
    
  - STM32MP215F-DK Set-up
    For a direct UART connection using the CN10 header on board MB2059-MP215F-A01, connect a 3.3 V TTL USB-to-UART adapter as follows:
    - CN10 pin 15 (EXP_GPIO22, UART4_TX.M33) to the adapter RX
    - CN10 pin 16 (EXP_GPIO23, UART4_RX.M33) to the adapter TX
    - Adapter GND to a GND pin on CN10
    Do not connect the adapter's VCC pin. Use a common ground.

    Alternatively, use VCP2 (UART4) through ST-LINK. Ensure that the board switch configuration routes UART4 between the target MPU and ST-LINK.

  - Open the serial terminal on the PC (for example, HyperTerminal or TeraTerm) and configure it for 115200 bauds, 8 data bits, 1 stop bit, no parity, and no hardware flow control.

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

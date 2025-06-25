/**
  @page I3C_Target_I2C_ComDMA I2C Two Boards Communication DMA example
  
  @verbatim
  ******************** (C) COPYRIGHT 2025 STMicroelectronics *******************
  * @file    I2C/I3C_Target_I2C_ComDMA/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the I2C Two Boards Communication DMA example.
  ******************************************************************************
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description 

How to handle I2C data buffer transmission/reception between two boards,
using DMA.

      - Board: STM32MP215-DK (embeds a STM32MP21xxxx device)
      - SCL Pin: PG13 (CN10, pin5)
      - SDA Pin: PA2  (CN10, pin3)

   _________________________                       _________________________ 
  |           ______________|                     |______________           |
  |          |I2C1          |                     |          I2C1|          |
  |          |              |                     |              |          |
  |          |          SCL |_____________________| SCL          |          |
  |          |              |                     |              |          |
  |          |              |                     |              |          |
  |          |              |                     |              |          |
  |          |          SDA |_____________________| SDA          |          |
  |          |              |                     |              |          |
  |          |______________|                     |______________|          |
  |                         |                     |                         |
  |                      GND|_____________________|GND                      |
  |_STM32_Board 1___________|                     |_STM32_Board 2___________|

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the systick.
Then the System clock source is configured by the SystemClock_Config() function in case of Engineering Mode, this clock configuration is done
by the Firmware running on the Cortex-M33 in case of Production Mode.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK, 
GPIO, DMA and NVIC). You may update this function to change I2C configuration.

The I2C communication is then initiated.

The project is split in two workspace:
the Controller Board (I3C_Controller_I2C_ComDMA) and the Target Board (I3C_Target_I2C_ComDMA)

- Target Board :
  The HAL_I2C_Slave_Receive_DMA() and the HAL_I2C_Slave_Transmit_DMA() functions
  allow respectively the reception and the transmission of a predefined data buffer
  in Target mode using interrupt.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button on the Controller Board,
I2C Controller starts the communication by sending aTxBuffer
to I2C Target which receives aRxBuffer through HAL_I2C_Slave_Receive_IT().
The second step starts when the user press the USER push-button on the Controller Board,
the I2C Target sends aTxBuffer through HAL_I2C_Slave_Transmit_IT()
to the I2C Controller which receives aRxBuffer.
The end of this two steps are monitored through the HAL_I2C_GetState() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to
check buffers correctness.

STM32MP215F-DK's LEDs can be used to monitor the transfer status:

 - LED3 will toggle when the transmission and reception process is complete.
 - LED3 is ON in case of error in initialization or transmission/reception process

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents 

  - I3C/I3C_Target_I2C_ComDMA/CM33/NonSecure/Inc/stm32mp215f_disco_conf.h   BSP configuration file
  - I2C/I3C_Target_I2C_ComDMA/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h      HAL configuration file
  - I2C/I3C_Target_I2C_ComDMA/CM33/NonSecure/Inc/stm32mp2xx_it.h            DMA and I2C interrupt handlers header file
  - I2C/I3C_Target_I2C_ComDMA/CM33/NonSecure/Inc/main.h                     Header for main.c module  
  - I2C/I3C_Target_I2C_ComDMA/CM33/NonSecure/Src/stm32mp2xx_it.c            DMA and I2C interrupt handlers
  - I2C/I3C_Target_I2C_ComDMA/CM33/NonSecure/Src/main.c                     Main program
  - I2C/I3C_Target_I2C_ComDMA/CM33/NonSecure/Src/system_stm32mp2xx.c        STM32MP2xx system source file
  - I2C/I3C_Target_I2C_ComDMA/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c       HAL MSP file    

@par Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.

  - This example has been tested with STM32MP215F-DK board and can be
    easily tailored to any other supported device and development board.

  - STM32MP215F-DK Set-up
    - Connect I2C_SCL line of Master board PG13(CN10, pin5) to I2C_SCL line of Slave Board PG13(CN10, pin5).
    - Connect I2C_SDA line of Master board PA2 (CN10, pin3) to I2C_SDA line of Slave Board PA2 (CN10, pin3).
    - Connect GND (CN10, pin39) of Master board to GND (CN10, pin39) of Slave Board.

@par How to use it ? 

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/I3C_Target_I2C_ComDMA/lib/firmware'
- scp the signed firmware I3C_Target_I2C_ComDMA_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/I3C_Target_I2C_ComDMA/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/I3C_Target_I2C_ComDMA/
- To run the example on target: 
	On target shell run:
	- cd /home/root/I3C_Target_I2C_ComDMA
	- ./fw_cortex_m33.sh start

 */

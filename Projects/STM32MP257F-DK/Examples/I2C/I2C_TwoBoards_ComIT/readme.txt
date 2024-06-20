/**
  @page I2C_TwoBoards_ComIT I2C Two Boards Communication IT example
  
  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    I2C/I2C_TwoBoards_ComIT/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the I2C Two Boards Communication IT example.
  ******************************************************************************
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

@par Example Description 

How to handle I2C data buffer transmission/reception between two boards, 
via Interrupt.

Board: STM32MP257F-DK (embeds a STM32MP257FAIx device)
SCL Pin: PZ4 (CN5, pin5)
SDA Pin: PZ3 (CN5, pin3)

   _________________________                       _________________________ 
  |           ______________|                     |______________           |
  |          |I2C8          |                     |          I2C8|          |
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
GPIO and NVIC). You may update this function to change I2C configuration.

The I2C communication is then initiated.
The project is split in two parts: the Master Board and the Slave Board
- Master Board
  The HAL_I2C_Master_Receive() and the HAL_I2C_Master_Transmit() functions 
  allow respectively the reception and the transmission of a predefined data buffer
  in Master mode.
- Slave Board
  The HAL_I2C_Slave_Receive() and the HAL_I2C_Slave_Transmit() functions 
  allow respectively the reception and the transmission of a predefined data buffer
  in Slave mode.
The user can choose between Master and Slave through "#define MASTER_BOARD"
in the "main.c" file:
If the Master board is used, the "#define MASTER_BOARD" must be uncommented.
If the Slave board is used the "#define MASTER_BOARD" must be commented.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the User USER2 push-button on the Master Board,
I2C Master starts the communication by sending aTxBuffer through HAL_I2C_Master_Transmit()
to I2C Slave which receives aRxBuffer through HAL_I2C_Slave_Receive(). 
The second step starts when the user press once again the User USER2 push-button on the Master Board,
the I2C Slave sends aTxBuffer through HAL_I2C_Slave_Transmit()
to the I2C Master which receives aRxBuffer through HAL_I2C_Master_Receive().
The end of this two steps are monitored through the HAL_I2C_GetState() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to 
check buffers correctness.  

STM32MP257F-DK's LED can be used to monitor the transfer status:
 - LED3 is slowly blinking (0.5 sec. period) when the transmission and reception process is complete.
 - LED3 is ON in case of error in initialization or transmission/reception process.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents 

  - I2C/I2C_TwoBoards_ComIT/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - I2C/I2C_TwoBoards_ComIT/CM33/NonSecure/Inc/stm32mp2xx_it.h          interrupt handlers header file
  - I2C/I2C_TwoBoards_ComIT/CM33/NonSecure/Inc/main.h                   Header for main.c module  
  - I2C/I2C_TwoBoards_ComIT/CM33/NonSecure/Src/stm32mp2xx_it.c          interrupt handlers
  - I2C/I2C_TwoBoards_ComIT/CM33/NonSecure/Src/main.c                   Main program
  - I2C/I2C_TwoBoards_ComIT/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file
  - I2C/I2C_TwoBoards_ComIT/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP file    

@par Hardware and Software environment

  - This example runs on STM32MP257FAIx devices.
    
  - This example has been tested with STM32MP257F-DK board and can be
    easily tailored to any other supported device and development board.    

  - STM32MP257F-DK Set-ups
    - Connect I2C_SCL line of Master board (PZ4, CN5, pin5) to I2C_SCL line of Slave Board (PZ4, CN5, pin5).
    - Connect I2C_SDA line of Master board (PZ3, CN5, pin3) to I2C_SDA line of Slave Board (PZ3, CN5, pin3).
    - Connect GND (CN5, pin39) of Master board to GND (CN5, pin39) of Slave Board.

@par How to use it ? 

In order to make the program work, you must do the following :
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
    o Uncomment "#define MASTER_BOARD" and load the project in Master Board
    o Comment "#define MASTER_BOARD" and load the project in Slave Board
- ssh root@192.168.7.1 'mkdir -p /home/root/I2C_TwoBoards_ComIT/lib/firmware'
- scp the firmware I2C_TwoBoards_ComIT_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/I2C_TwoBoards_ComIT/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/I2C_TwoBoards_ComIT/
- To run the example on target: 
	On target shell run:
	- cd /home/root/I2C_TwoBoards_ComIT
	- ./fw_cortex_m33.sh start

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

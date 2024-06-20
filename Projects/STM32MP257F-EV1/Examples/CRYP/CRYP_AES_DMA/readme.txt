/**
  @page CRYP_AES_DMA  Encrypt and Decrypt data using AES Algo in ECB chaining
   modes using DMA
  
  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    CRYP/CRYP_AES_DMA/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the CRYP AES-128 Algo in ECB mode with DMA Example
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

@par Example Description

This example provides a short description of how to use the CRYPTO peripheral to 
encrypt and decrypt data using AES-128 Algorithm with ECB chaining mode.

For this example, DMA is used to transfer data from memory to the CRYPTO
processor IN FIFO and also to transfer data from CRYPTO processor OUT FIFO
to memory.

The data to be encrypted is a 256-bit data (8 words), which correspond to
2x AES-128 Blocks.

This example behaves as below: 
- AES Encryption (Plain Data --> Encrypted Data)
- AES Key preparation for decryption (Encryption Key --> Decryption Key)
- AES Decryption (Encrypted Data --> Decrypted Data)

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the systick.
Then the System clock is configured at 650 MHz.

STM32MP257F-EV1 LEDs can be used to monitor the transfer status:
 - LED3 is blinking when encryption/decryption and TAG generation is right.
 - LED3 is ON when encryption or decryption or TAG generation is wrong.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Security, Cryptography, CRYPT, AES, DMA, MAC, Cypher, ECB, Key

@par Directory contents 

  - CRYP/CRYP_AES_DMA/CM33/NonSecure/Inc/main.h                   Header for main.c module  
  - CRYP/CRYP_AES_DMA/CM33/NonSecure/Inc/stm32mp257f_eval_conf.h  Interrupt handlers
  - CRYP/CRYP_AES_DMA/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - CRYP/CRYP_AES_DMA/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - CRYP/CRYP_AES_DMA/CM33/NonSecure/Src/main.c                   Main program
  - CRYP/CRYP_AES_DMA/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP module
  - CRYP/CRYP_AES_DMA/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers

@par Hardware and Software environment

  - This example runs on STM32MP257FAIx devices.
  
  - This example has been tested with an STMicroelectronics STM32MP257F-EV1
    board and can be easily tailored to any other supported device 
    and development board.

@par How to use it ?
 
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/CRYP_AES_DMA/lib/firmware'
- scp the signed firmware CRYP_AES_DMA_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/CRYP_AES_DMA/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/CRYP_AES_DMA/
- To run the example on target: 
	On target shell run:
	- cd /home/root/CRYP_AES_DMA
	- ./fw_cortex_m33.sh start


 */

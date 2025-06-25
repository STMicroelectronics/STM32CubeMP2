/**
  @page EXTI_ToggleLedOnIT_Init EXTI example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/EXTI/EXTI_ToggleLedOnIT_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the EXTI example.
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

How to configure the EXTI and use GPIOs to toggle the user LEDs
available on the board when a user button is pressed. This example is 
based on the STM32MP2xx LL API. The peripheral initialization uses 
LL initialization functions to demonstrate LL init usage.

At the beginning of the main program the SystemClock_Config() function is called to configure the system clock for STM32MP21xxxx Devices :
The CPU at 520 MHz.
The HCLK for D1 Domain AXI and AHB3 peripherals, D2 Domain AHB1/AHB2 peripherals and D3 Domain AHB4 peripherals at 520 MHz/2.
The APB clock dividers for D1 Domain APB3 peripherals, D2 Domain APB1 and APB2 peripherals and D3 Domain APB4 peripherals to run at 520 MHz/4.

In this example, one EXTI line (EXTI2_8) is configured to generate
an interrupt on each falling edge.

In the interrupt routine a led connected to a specific GPIO pin is toggled.

In this example:
    - EXTI2_8 is connected to PG.8 pin
      - when falling edge is detected on EXTI2_8 by pressing User push-button, LED3 toggles
    - In case of error, LED3 tunrs ON continuously.

On STM32MP21xx_DK:
    - EXTI2_8 is connected to User push-button


@Note If the  application is using the DTCM/ITCM memories (@0x20000000/0x0000000: not cacheable and only accessible
      by the Cortex M7 and the MDMA), no need for cache maintenance when the Cortex M7 and the MDMA access these RAMs.
      If the application needs to use DMA(or other masters) based access or requires more RAM, then the user has to:
              - Use a non TCM SRAM. (example : D1 AXI-SRAM @ 0x24000000)
              - Add a cache maintenance mechanism to ensure the cache coherence between CPU and other masters (DMAs, DMA2D, LTDC, MDMA).
              - The addresses and the size of cacheable buffers (shared between CPU and other masters)
                must be properly defined to be aligned to L1-CACHE line size (32 bytes).

@Note It is recommended to enable the cache and maintain its coherence.

@par Keywords

System, GPIO, Output, EXTI, Button, LED, Toggle

@par Directory contents 

  - EXTI/EXTI_ToggleLedOnIT_Init/Inc/STM32MP2xx_it.h          Interrupt handlers header file
  - EXTI/EXTI_ToggleLedOnIT_Init/Inc/main.h                   Header for main.c module
  - EXTI/EXTI_ToggleLedOnIT_Init/Src/STM32MP2xx_it.c          Interrupt handlers
  - EXTI/EXTI_ToggleLedOnIT_Init/Src/main.c                   Main program
  - EXTI/EXTI_ToggleLedOnIT_Init/Src/system_STM32MP2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.
    
  - This example has been tested with STM32MP215xx-DK board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?
 
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/EXTI_ToggleLedOnIT_Init/lib/firmware'
- scp the signed firmware EXTI_ToggleLedOnIT_Init_NonSecure_sign.bin to root@192.168.7.1:/home/root/EXTI_ToggleLedOnIT_Init/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/EXTI_ToggleLedOnIT_Init/
- To run the example on target: 
	On target shell run:
	- cd /home/root/EXTI_ToggleLedOnIT_Init
	- ./fw_cortex_m33.sh start


 */
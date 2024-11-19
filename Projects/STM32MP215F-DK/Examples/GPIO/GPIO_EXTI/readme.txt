/**
  @page GPIO_EXTI GPIO EXTI example
  
  @verbatim
    ******************** (C) COPYRIGHT 2024 STMicroelectronics *****************
  * @file    GPIO/GPIO_EXTI/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the GPIO EXTI example.
  ******************************************************************************
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  @endverbatim

@par Example Description

How to configure external interrupt lines.

In this example, one EXTI line (EXTI2_11) is configured to generate
an interrupt on each falling edge.
In the interrupt routine a led connected to a specific GPIO pin is toggled.

In this example:
    - EXTI2_11 is connected to PC11 pin
      - when falling edge is detected on EXTI2_11 by pressing USER2 push-button, LED3 toggles once
      - LED3 is ON in case of error in initialization

On STM32MP215F-DK:
    - EXTI2_11 is connected to USER2 push-button

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the systick.
Then the System clock source is configured by the SystemClock_Config() function in case of Engineering Mode, this clock configuration is done
by the Firmware running on the Cortex-A35 in case of Production Mode.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, GPIO, EXTI, Output, Alternate function, Push-pull, Toggle

@par Directory contents 

  - GPIO/GPIO_EXTI/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - GPIO/GPIO_EXTI/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - GPIO/GPIO_EXTI/CM33/NonSecure/Inc/main.h                   Header for main.c module  
  - GPIO/GPIO_EXTI/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - GPIO/GPIO_EXTI/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP file
  - GPIO/GPIO_EXTI/CM33/NonSecure/Src/main.c                   Main program
  - GPIO/GPIO_EXTI/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP215Fxx devices.
    
  - This example has been tested with STM32MP215F-DK board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/GPIO_EXTI/lib/firmware'
- scp the firmware GPIO_EXTI_CM33_NonSecure.elf to root@192.168.7.1:/home/root/GPIO_EXTI/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/GPIO_EXTI/
- To run the example on target: 
	On target shell run:
	- cd /home/root/GPIO_EXTI
	- ./fw_cortex_m33.sh start

 */

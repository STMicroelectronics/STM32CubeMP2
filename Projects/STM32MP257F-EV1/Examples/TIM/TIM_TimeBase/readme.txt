/**
  @page TIM_TimeBase TIM_TimeBase example
  
  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    TIM/TIM_TimeBase/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM DMA Burst example.
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

This example shows how to configure the TIM peripheral to generate a time base of
one second with the corresponding Interrupt request.

    In this example TIM3 input clock (TIM3CLK)  is set to 2 * APB1 clock (PCLK1),
    since APB1 prescaler is equal to 0.
      TIM3CLK = 2 * PCLK1
      PCLK1 = HCLK / 2
      => TIM3CLK = HCLK = SystemCoreClock
    To get TIM3 counter clock at 10 KHz, the Prescaler is computed as following:
    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    Prescaler = (SystemCoreClock /10 KHz) - 1

SystemCoreClock is set to 209 MHz for STM32MP257FAIx Devices.

The TIM3 ARR register value is equal to 10000 - 1,
Update rate = TIM3 counter clock / (Period + 1) = 1 Hz,
So the TIM3 generates an interrupt each 1 s

When the counter value reaches the auto-reload register value, the TIM update
interrupt is generated and, in the handler routine, pin PJ.06 (connected to LED3 on board STM32MP257F-EV1)
is toggled. So led blinks at the following frequency: 0.5Hz.
LED3 is ON in case of error in initialization.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents  

  - TIM/TIM_TimeBase/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - TIM/TIM_TimeBase/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - TIM/TIM_TimeBase/CM33/NonSecure/Inc/main.h                   Header for main.c module  
  - TIM/TIM_TimeBase/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - TIM/TIM_TimeBase/CM33/NonSecure/Src/main.c                   Main program
  - TIM/TIM_TimeBase/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP file
  - TIM/TIM_TimeBase/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP257FAIx devices.

  - This example has been tested with STMicroelectronics STM32MP257F-EV1
    board and can be easily tailored to any other supported device
    and development board.

  - STM32MP257F-EV1 Set-up
    - Use LED3 to get visual verdict of the test (LED3 blinks at 0.5Hz) and connect R16 resistor
      to an oscilloscope to show the Time Base signal.

@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/TIM_TimeBase/lib/firmware'
- scp the signed firmware TIM_TimeBase_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/TIM_TimeBase/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/TIM_TimeBase/
- To run the example on target: 
	On target shell run:
	- cd /home/root/TIM_TimeBase
	- ./fw_cortex_m33.sh start

 */

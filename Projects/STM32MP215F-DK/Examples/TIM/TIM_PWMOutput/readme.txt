/**
  @page TIM_PWMOutput TIM_PWMOutput example
  
  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    TIM/TIM_PWMOutput/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM PWM Output example.
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

This example shows how to configure the TIM peripheral in PWM (Pulse Width Modulation)
mode.

SystemCoreClock is set to 209 MHz for STM32MP21xxxx Devices.

    In this example TIM8 input clock (TIM8CLK) is set to 2 * APB2 clock (PCLK2),
    since APB2 prescaler is equal to 0.
      TIM8CLK = 2 * PCLK2
      PCLK2 = HCLK / 2
      => TIM8CLK = HCLK = SystemCoreClock

    To get TIM8 counter clock at 16 MHz, the prescaler is computed as follows:
       Prescaler = (TIM8CLK / TIM8 counter clock) - 1
       Prescaler = ((SystemCoreClock) /16 MHz) - 1

    To get TIM8 output clock at 24 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM8 counter clock / TIM8 output clock) - 1
           = 665

    TIM8 Channel2 duty cycle = (TIM8_CCR2/ TIM8_ARR + 1)* 100 = 50%
    TIM8 Channel3 duty cycle = (TIM8_CCR3/ TIM8_ARR + 1)* 100 = 25%
    TIM8 Channel4 duty cycle = (TIM8_CCR4/ TIM8_ARR + 1)* 100 = 12.5%

STM32MP215F-'s LED can be used to monitor the status:
 - LED3 is ON in case of error in initialization.
 
The PWM waveforms can be displayed using an oscilloscope.

@note The duty cycles values mentioned above are theoretical (obtained when the system clock frequency is exactly 209 MHz).
      They might be slightly different depending on system clock frequency precision.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents  

  - TIM/TIM_PWMOutput/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - TIM/TIM_PWMOutput/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - TIM/TIM_PWMOutput/CM33/NonSecure/Inc/main.h                   Header for main.c module  
  - TIM/TIM_PWMOutput/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - TIM/TIM_PWMOutput/CM33/NonSecure/Src/main.c                   Main program
  - TIM/TIM_PWMOutput/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP file
  - TIM/TIM_PWMOutput/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.

  - This example has been tested with STMicroelectronics STM32MP215F-DK
    board and can be easily tailored to any other supported device
    and development board.

  - STM32MP215-DK Set-up
    - Use MB1936 motherboard.
    - Connect the following pins to an oscilloscope to monitor the different waveforms:
          - TIM8_CH2 : PC.08 (pin 32 in CN10 connector)
          - TIM8_CH3 : PC.12 (pin 28 in CN10 connector)
          - TIM8_CH4 : PC.10 (pin 33 in CN10 connector)

@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/TTIM_PWMOutput/lib/firmware'
- scp the signed firmware TTIM_PWMOutput_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/TTIM_PWMOutput/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/TTIM_PWMOutput/
- To run the example on target: 
	On target shell run:
	- cd /home/root/TTIM_PWMOutput
	- ./fw_cortex_m33.sh start

 */

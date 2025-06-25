/**
  @page TIM_DMABurst TIM_DMABurst example
  
  @verbatim
  ******************** (C) COPYRIGHT 2023 STMicroelectronics *******************
  * @file    TIM/TIM_DMABurst/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM DMA Burst example.
  ******************************************************************************
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

This example shows how to update the TIMER TIM8_CH4 period and the duty cycle 
using the TIMER DMA burst feature.

Every update DMA request, the DMA will do 6 transfers of words into Timer 
registers beginning from ARR register.
On the DMA update request, 0x0FFF will be transferred into ARR, 0x0000 will be transferred into RCR (if supported),
 0x0000 will be transferred into CCR1, 0x0000 will be transferred into CCR2,
 0x0000 will be transferred into CCR3, 0x0555 will be transferred into CCR4. 

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the systick.
Then the System clock source is configured by the SystemClock_Config() function in case of Engineering Mode, 
this clock configuration is done by the Firmware running on the Cortex-A35 in case of Production Mode.

This Peripheral clock frequency is get with RCC_PERIPHCLK_TIM8.
To get TIM8 counter clock at 16 MHz the Prescaler is computed as following:
- Prescaler = (TIM8CLK / TIM8 counter clock) - 1

The TIM8 Frequency = TIM8 counter clock/(ARR + 1)
                   = 16 MHz / 4096 = 3.9 KHz

The TIM8 CCR4 register value is equal to 0x555, so the TIM8 Channel 4 generates a 
PWM signal with a frequency equal to 3.9 KHz and a duty cycle equal to 33.33%:
TIM8 Channel4 duty cycle = (TIM8_CCR4/ TIM8_ARR + 1)* 100 = 33.33%

The PWM waveform can be displayed using an oscilloscope.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents  

  - TIM/TIM_DMABurst/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - TIM/TIM_DMABurst/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - TIM/TIM_DMABurst/CM33/NonSecure/Inc/main.h                   Header for main.c module  
  - TIM/TIM_DMABurst/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - TIM/TIM_DMABurst/CM33/NonSecure/Src/main.c                   Main program
  - TIM/TIM_DMABurst/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP file
  - TIM/TIM_DMABurst/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.
    
  - This example has been tested with STMicroelectronics STM32MP215F-DK
    board and can be easily tailored to any other supported device 
    and development board.

  - STM32MP215F-DK Set-up
    - Connect the TIM8 output channel to an oscilloscope to monitor the different waveforms: 
    - TIM8 TIM_CHANNEL_4 (PC.10 (pin 33 in CN10 connector))

@par How to use it ?
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/TIM_DMABurst/lib/firmware'
- scp the signed firmware TIM_DMABurst_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/TIM_DMABurst/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/TIM_DMABurst/
- To run the example on target: 
	On target shell run:
	- cd /home/root/TIM_DMABurst
	- ./fw_cortex_m33.sh start

 */

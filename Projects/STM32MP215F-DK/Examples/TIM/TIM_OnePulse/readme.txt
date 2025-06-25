--/**
  @page TIM_OnePulse TIM_OnePulse example
  
  @verbatim
  ******************** (C) COPYRIGHT 2023 STMicroelectronics *******************
  * @file    TIM/TIM_OnePulse/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM One Pulse example.      
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

This example shows how to use the TIMER peripheral to generate a single pulse when
a rising edge of an external signal is received on the TIMER Input pin.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32MP21xxxx Devices :
The CPU at 400MHz 
The HCLK for D1 Domain AXI and AHB3 peripherals , D2 Domain AHB1/AHB2 peripherals and D3 Domain AHB4  peripherals at 200MHz.
The APB clock dividers for D1 Domain APB3 peripherals, D2 Domain APB1 and APB2 peripherals and D3 Domain APB4 peripherals to  run at 100MHz
Clock setup for TIM5

================================

  TIM5CLK = 2*PCLK2 = 200 MHz.
  
  Prescaler = (TIM5CLK /TIM5 counter clock) - 1
  
  The prescaler value is computed in order to have TIM5 counter clock 
  set at 20000000 Hz.
  
  The Autoreload value is 65535 (TIM5->ARR), so the maximum frequency value to 
  trigger the TIM5 input is 20000000/65535 [Hz].
 
Configuration of TIM5 in One Pulse Mode
===================================================
 
  - The external signal is connected to TIM5_CH2 pin (PI.5), 
    and a rising edge on this input is used to trigger the Timer.
  - The One Pulse signal is output on TIM5_CH1 (PH.8).

  The delay value is fixed to:
   - Delay =  CCR1/TIM5 counter clock 
           = 16383 / 20000000 [sec]
           
  The pulse value is fixed to : 
   - Pulse value = (TIM_Period - TIM_Pulse)/TIM5 counter clock  
                 = (65535 - 16383) / 20000000 [sec]

  The one pulse waveform can be displayed using an oscilloscope and it looks
  like this.
  
                                ___
                               |   |
  CH2 _________________________|   |__________________________________________
 
                                             ___________________________
                                            |                           |
  CH1 ______________________________________|                           |_____
                               <---Delay----><------Pulse--------------->
  

STM32MP215F-DK's LED can be used to monitor the status:
 - LED3 is ON in case of error in initialization.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function. 
 
@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Timer, Output, signals, One Pulse, PWM, Oscilloscope, External signal, Autoreload, Waveform

@par Directory contents  

  - TIM/TIM_OnePulse/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - TIM/TIM_OnePulse/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - TIM/TIM_OnePulse/CM33/NonSecure/Inc/main.h                   Header for main.c module  
  - TIM/TIM_OnePulse/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - TIM/TIM_OnePulse/CM33/NonSecure/Src/main.c                   Main program
  - TIM/TIM_OnePulse/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP file
  - TIM/TIM_OnePulse/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.

  - This example has been tested with STMicroelectronics STM32MP215F-DK 
    board and can be easily tailored to any other supported device 
    and development board.

  - STM32MP215F-DK Set-up
    - Connect the external signal to the TIM5_CH2 pin (PI.5) (pin 19 in CN10 connector)
    - Connect the TIM5_CH1 pin(PH.8) (pin 16 in CN10 connector) to an oscilloscope to monitor the waveform. 

@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/TIM_OnePulse/lib/firmware'
- scp the signed firmware TIM_OnePulse_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/TIM_OnePulse/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/TIM_OnePulse/
- To run the example on target: 
	On target shell run:
	- cd /home/root/TIM_OnePulse
	- ./fw_cortex_m33.sh start

 */

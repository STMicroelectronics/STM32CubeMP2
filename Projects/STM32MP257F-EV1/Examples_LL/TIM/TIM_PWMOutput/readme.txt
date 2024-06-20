/**
  @page TIM_PWMOutput TIM example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_PWMOutput/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM_PWMOutput example.
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

Use of a timer peripheral to generate a 
PWM output signal and update the PWM duty cycle. This example is based on the 
STM32MP2xx TIM LL API. The peripheral initialization uses 
LL unitary service functions for optimization purposes (performance and size).

At the beginning of the main program the SystemClock_Config() function is called to configure the system clock for STM32MP257FAIx Devices :
The CPU at 520 MHz.
The HCLK for D1 Domain AXI and AHB3 peripherals, D2 Domain AHB1/AHB2 peripherals and D3 Domain AHB4 peripherals at 520 MHz/2.
The APB clock dividers for D1 Domain APB3 peripherals, D2 Domain APB1 and APB2 peripherals and D3 Domain APB4 peripherals to run at 520 MHz/4.

In this example TIM8 input clock (TIM8CLK) frequency is set to twice the APB1 clock (PCLK1),
since APB1 pre-scaler is equal to 2.
    TIM3CLK = 2*PCLK1
    PCLK1 = HCLK/2
    => TIM8CLK = HCLK = SystemCoreClock/AHB_DIV = 520 MHz/2 = 260 MHz

To set the TIM8 counter clock frequency to 10 KHz, the pre-scaler (PSC) is
calculated as follows:
PSC = (TIM8CLK / TIM8 counter clock) - 1
PSC = ((SystemCoreClock/2) / 10 KHz) - 1

SystemCoreClock is set to 520 MHz for STM32MP2xx Devices.

Auto-reload (ARR) is calculated to get a time base period of 10 ms,
meaning a time base frequency of 100 Hz.
ARR = (TIM8 counter clock / time base frequency) - 1
ARR = (TIM8 counter clock / 100) - 1

Initially, the capture/compare register (CCR1) of the output channel is set to
half the auto-reload value meaning a initial duty cycle of 50%.
Generally speaking this duty cycle is calculated as follows:
Duty cycle = (CCR1 / ARR) * 100

The timer output channel is mapped on the pin PB.00 (connected to LED1 on board
STM32MP25xx_EV1).

User push-button can be used to change the duty cycle from 0% up to 100% by
steps of 10%. Duty cycle is periodically measured. It can be observed through
the debugger by watching the variable uwMeasuredDutyCycle.

Initially the output channel is configured in output compare toggle mode.
@note The GPIOJ5 is not toggling. In fact, if the dutycycle is 0% so the GPIOJ5 is LOW. When pushing 
successively the user button, the GPIOJ5 is HIGH and its luminosity rises as the dutycycle value keeps 
increasing.


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

  - TIM/TIM_PWMOutput/Inc/STM32MP2xx_it.h          Interrupt handlers header file
  - TIM/TIM_PWMOutput/Inc/main.h                   Header for main.c module
  - TIM/TIM_PWMOutput/Src/STM32MP2xx_it.c          Interrupt handlers
  - TIM/TIM_PWMOutput/Src/main.c                   Main program
  - TIM/TIM_PWMOutput/Src/system_STM32MP2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP257FAIx devices.

  - This example has been tested with STMicroelectronics STM32MP257F-EV1
    board and can be easily tailored to any other supported device
    and development board.

  - STM32MP257F-EV1 Set-up
    - Use MB1936 motherboard.
    - Connect the following pins to an oscilloscope to monitor the different waveforms:
          - TIM8_CH1 : PJ.05 (pin 33 in CN5 connector)
          
@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/TIM_PWMOutput/lib/firmware'
- scp the signed firmware TTIM_PWMOutput_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/TIM_PWMOutput/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/TIM_PWMOutput/
- To run the example on target: 
	On target shell run:
	- cd /home/root/TIM_PWMOutput
	- ./fw_cortex_m33.sh start

 */

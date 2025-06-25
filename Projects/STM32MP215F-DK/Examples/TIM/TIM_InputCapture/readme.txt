/**
  @page TIM_InputCapture Input Capture example
  
  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    TIM/TIM_InputCapture/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM_InputCapture example.
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

  This example shows how to use the TIM peripheral to measure the frequency of
  an external signal.


@par Keywords

Timer, DMA, Frequency, Input, Capture, External Signal, Measurement
@par Directory contents  

  - TIM/TIM_InputCapture/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - TIM/TIM_InputCapture/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - TIM/TIM_InputCapture/CM33/NonSecure/Inc/main.h                   Header for main.c module  
  - TIM/TIM_InputCapture/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - TIM/TIM_InputCapture/CM33/NonSecure/Src/main.c                   Main program
  - TIM/TIM_InputCapture/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP file
  - TIM/TIM_InputCapture/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.

  - This example has been tested with STMicroelectronics STM32MP215F-DK
    board and can be easily tailored to any other supported device
    and development board.

  - STM32MP215F-DK Set-up
    - Connect an external signal to TIM8 Channel4 (PC10 Pin33 of CN10).

@par How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/TIM_InputCapture/lib/firmware'
- scp the firmware TTIM_InputCapture_CM33_NonSecure.bin to root@192.168.7.1:/home/root/TIM_InputCapture/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/TIM_InputCapture/
- To run the example on target: 
	On target shell run:
	- cd /home/root/TIM_InputCapture
	- ./fw_cortex_m33.sh start

 */

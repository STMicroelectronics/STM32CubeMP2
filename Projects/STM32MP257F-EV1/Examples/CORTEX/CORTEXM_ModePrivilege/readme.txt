/**
  @page CORTEXM_ModePrivilege example
  
  @verbatim
  ******************** (C) COPYRIGHT 2023 STMicroelectronics *******************
  * @file    Examples/CORTEX/CORTEXM_ModePrivilege/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the CORTEXM_ModePrivilege example.
  ******************************************************************************
  * @attention
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

How to modify the Thread mode privilege access and stack. Thread mode is entered
on reset or when returning from an exception.

The associated program is used to:

1. Switch the Thread mode stack from Main stack to Process stack

2. Switch the Thread mode from Privileged to Unprivileged

3. Switch the Thread mode from Unprivileged back to Privileged

To monitor the stack used and the privileged or unprivileged access level of code
in Thread mode, a set of variables is available within the program. It is also
possible to use the 'Cortex register' window of the debugger.

LED3 Blinks with period 1 sec when the test is finished successfully.
LED3 is ON when there is an error in initialization.

@par Notes

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

@par Keywords

System, Cortex, Mode Privilege, Privileged access, thread mode, main stack, process stack

@par Directory contents

  - CORTEX/CORTEXM_ModePrivilege/CM33/NonSecure/Inc/stm32mp257f_eval_conf.h     BSP configuration file
  - CORTEX/CORTEXM_ModePrivilege/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h       HAL configuration file
  - CORTEX/CORTEXM_ModePrivilege/CM33/NonSecure/Inc/stm32mp2xx_it.h             Interrupt handlers header file
  - CORTEX/CORTEXM_ModePrivilege/CM33/NonSecure/Inc/main.h                      Header for main.c module
  - CORTEX/CORTEXM_ModePrivilege/CM33/NonSecure/Src/stm32mp2xx_it.c             Interrupt handlers
  - CORTEX/CORTEXM_ModePrivilege/CM33/NonSecure/Src//stm32mp2xx_hal_msp.c       HAL MSP file
  - CORTEX/CORTEXM_ModePrivilege/CM33/NonSecure/Src/main.c                      Main program
  - CORTEX/CORTEXM_ModePrivilege/CM33/NonSecure/Src/system_stm32mp2xx.c         STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP257FAIx devices.

  - This example has been tested with STM32MP257F-EV1 board and can be
    easily tailored to any other supported device and development board.      

@par How to use it ?
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/CORTEXM_ModePrivilege/lib/firmware'
- scp the signed firmware CORTEXM_ModePrivilege_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/CORTEXM_ModePrivilege/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/CORTEXM_ModePrivilege/
- To run the example on target: 
	On target shell run:
	- cd /home/root/CORTEXM_ModePrivilege
	- ./fw_cortex_m33.sh start

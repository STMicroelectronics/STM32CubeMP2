/**
  @page CRC_UserDefinedPolynomial  Example

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    CRC/CRC_UserDefinedPolynomial/readme.txt
  * @author  MCD Application Team
  * @brief   Description of CRC Example with user-defined generating polynomial.
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

How to configure the CRC using the HAL API. The CRC (cyclic redundancy check) calculation unit computes the 8-bit CRC code for a given
buffer of 32-bit data words, based on a user-defined generating polynomial.
In this example, the polynomial is set manually to 0x9B, that is, X^8 + X^7 + X^4 + X^3 + X + 1.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK).

The CRC peripheral configuration is ensured by HAL_CRC_Init() function.
The latter is calling HAL_CRC_MspInit() function which core is implementing the configuration of the needed CRC resources according to the used hardware (CLOCK).
You can update HAL_CRC_Init() input parameters to change the CRC configuration.

The calculated CRC code is stored in uwCRCValue variable.
Once calculated, the CRC value (uwCRCValue) is compared to the CRC expected value (uwExpectedCRCValue).

STM32 board LEDs are used to monitor the example status:

  - LED3 will toggle when the correct CRC value is calculated
  - LED3 is ON when there is an error in initialization or if an incorrect CRC value is calculated.

#### Notes

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### Keywords

Security, CRC, CRC Polynomial, CRC-8-WCDMA, hardware CRC, user-defined generating polynomial

### <b>Directory contents</b>

  - CRC/CRC_UserDefinedPolynomial/CM33/NonSecure/Inc/stm32mp215f_disco_conf.h  BSP configuration file
  - CRC/CRC_UserDefinedPolynomial/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h    HAL configuration file
  - CRC/CRC_UserDefinedPolynomial/CM33/NonSecure/Inc/stm32mp2xx_it.h          Interrupt handlers header file
  - CRC/CRC_UserDefinedPolynomial/CM33/NonSecure/Inc/main.h                   Header for main.c module
  - CRC/CRC_UserDefinedPolynomial/CM33/NonSecure/Src/stm32mp2xx_it.c          Interrupt handlers
  - CRC/CRC_UserDefinedPolynomial/CM33/NonSecure/Src/main.c                   Main program
  - CRC/CRC_UserDefinedPolynomial/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c     HAL MSP module
  - CRC/CRC_UserDefinedPolynomial/CM33/NonSecure/Src/system_stm32mp2xx.c      STM32MP2xx system source file

### Hardware and Software environment

  - This example runs on STM32MP21xxxx devices.
  - This example has been tested with STM32MP215F-DK board and can be
    easily tailored to any other supported device and development board.

### How to use it ?

In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/CRC_UserDefinedPolynomial/lib/firmware'
- scp the signed firmware CRC_UserDefinedPolynomial_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/CRC_UserDefinedPolynomial/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/CRC_UserDefinedPolynomial/
- To run the example on target:
	On target shell run:
	- cd /home/root/CRC_UserDefinedPolynomial
	- ./fw_cortex_m33.sh start

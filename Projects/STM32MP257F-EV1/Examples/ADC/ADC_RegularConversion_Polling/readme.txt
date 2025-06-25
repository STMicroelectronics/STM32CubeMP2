/**
  @page ADC_RegularConversion_Polling conversion using Polling

  @verbatim
  ******************** (C) COPYRIGHT 2024 STMicroelectronics *******************
  * @file    ADC/ADC_RegularConversion_Polling/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC RegularConversion Polling example.
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

This example describes how to use the ADC in Polling mode to convert data through the HAL API.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32 Device :
The CPU at 400MHz 
The HCLK for D1 Domain AXI and AHB3 peripherals , D2 Domain AHB1/AHB2 peripherals and D3 Domain AHB4  peripherals at 200MHz.
The APB clock dividers for D1 Domain APB3 peripherals, D2 Domain APB1 and APB2 peripherals and D3 Domain APB4 peripherals to  run at 100MHz

When the end of conversion occurs, the converted data of ADC3 DR register is 
copied to the uwConvertedValue variable.

Note that the test carries out only one conversion. To trigger another conversion, the user
needs to reset then restart the example execution. 

In this example, the system clock is 400MHz, AHB = 200MHz and ADC clock = AHB/4. 
Since ADC clock is 50 MHz and sampling time is set to 8.5 cycles, the conversion 
time to 16bit data is 16.5 cycles so the total conversion time is (16.5+8.5)/50 = 0.5us(2.0Msps).

Use an external power supply, adjust supply voltage and connect it to pin PF.11 (Pin 7 in CN5 Connector).
This allows to modify ADC_CHANNEL_6 voltage.

uwConvertedValue variable contains the 32-bit conversion result and 
uwInputVoltage yields the unsigned input voltage in mV.
The converted value is monitored through debugger: uwConvertedValue and uwInputVoltage variables.

STM32 board LED can be used to monitor the conversion:
  - LED3 is ON when there is an error during process.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The example needs to ensure that the SysTick time base is always set to 1 millisecond
 to have correct HAL operation.

@par Keywords

Analog, ADC, Analog to Digital Converter, Regular Conversion, Polling, Single Conversion, Voltage

@par Directory contents 
  
  - ADC/ADC_RegularConversion_Polling/CM33/NonSecure/Inc/stm32mp257f_eval_conf.h BSP configuration file
  - ADC/ADC_RegularConversion_Polling/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h   HAL configuration file
  - ADC/ADC_RegularConversion_Polling/CM33/NonSecure/Inc/stm32mp2xx_it.h         Interrupt handlers header file
  - ADC/ADC_RegularConversion_Polling/CM33/NonSecure/Inc/main.h                  Header for main.c module
  - ADC/ADC_RegularConversion_Polling/CM33/NonSecure/Src/stm32mp2xx_it.c         Interrupt handlers
  - ADC/ADC_RegularConversion_Polling/CM33/NonSecure/Src/main.c                  Main program
  - ADC/ADC_RegularConversion_Polling/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c    HAL MSP module
  - ADC/ADC_RegularConversion_Polling/CM33/NonSecure/Src/system_stm32mp2xx.c     STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP25xx devices.
  - This example has been tested with STM32MP257F-EV1 board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?
 
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/ADC_RegularConversion_Polling/lib/firmware'
- scp the signed firmware ADC_RegularConversion_Polling_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/ADC_RegularConversion_Polling/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/ADC_RegularConversion_Polling/
- To run the example on target: 
	On target shell run:
	- cd /home/root/ADC_RegularConversion_Polling
	- ./fw_cortex_m33.sh start

 */
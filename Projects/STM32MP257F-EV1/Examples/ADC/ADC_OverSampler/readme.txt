/**
  @page ADC_OverSampler ADC OverSampler Example
  
  @verbatim
  ******************************************************************************
  * @file    ADC/ADC_OverSampler/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC Oversampler example.
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

This example describes how to configure and use the ADC to convert an external
analog input combined with oversampling feature to increase resolution through the HAL API.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32 Device :
The CPU at 400MHz 
The HCLK for D1 Domain AXI and AHB3 peripherals , D2 Domain AHB1/AHB2 peripherals and D3 Domain AHB4  peripherals at 200MHz.
The APB clock dividers for D1 Domain APB3 peripherals, D2 Domain APB1 and APB2 peripherals and D3 Domain APB4 peripherals to  run at 100MHz

The ADC is configured to convert continuously ADC_CHANNEL_6 connected to PF.11 (Pin 7 in CN5 Connector)
while the oversampler is enabled. 
Oversampling ratio is set to 1024 so the maximum output result 
is 0xFFF * 1024 = 0x3FFC00 (22 bits). Result is 6-bit right shift.

uwConvertedValue variable contains the 16-bit conversion result and 
uwInputVoltage yields the input voltage in mV. 

Use an external power supply, adjust supply voltage and connect it to pin PF.11 (Pin 7 in CN5 Connector).
This allows to modify ADC_CHANNEL_6 voltage.
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

Analog, ADC, Analog to Digital, oversampling, Continuous conversion, Measurement, Voltage

@par Directory contents 
  
  - ADC/ADC_OverSampler/CM33/NonSecure/Inc/stm32mp257f_eval_conf.h BSP configuration file
  - ADC/ADC_OverSampler/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h   HAL configuration file
  - ADC/ADC_OverSampler/CM33/NonSecure/Inc/stm32mp2xx_it.h         Interrupt handlers header file
  - ADC/ADC_OverSampler/CM33/NonSecure/Inc/main.h                  Header for main.c module
  - ADC/ADC_OverSampler/CM33/NonSecure/Src/stm32mp2xx_it.c         Interrupt handlers
  - ADC/ADC_OverSampler/CM33/NonSecure/Src/main.c                  Main program
  - ADC/ADC_OverSampler/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c    HAL MSP module
  - ADC/ADC_OverSampler/CM33/NonSecure/Src/system_stm32mp2xx.c     STM32MP2xx system source file

@par Hardware and Software environment

  - This example runs on STM32MP25xx devices.
  - This example has been tested with STM32MP257F-EV1 board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?
 
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/ADC_OverSampler/lib/firmware'
- scp the signed firmware ADC_OverSampler_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/ADC_OverSampler/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/ADC_OverSampler/
- To run the example on target: 
	On target shell run:
	- cd /home/root/ADC_OverSampler
	- ./fw_cortex_m33.sh start

 */
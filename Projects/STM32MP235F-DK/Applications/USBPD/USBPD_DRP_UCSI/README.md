## <b>USBPD_DRP_UCSI Application Description</b>

This application provides an example running on STM32MP23xx Cortex-M33 acting as an **UCSI** (USB Type-CTM Connector System Software Interface) **Platform Policy Manager** (**PPM**). 
The PPM is a combination of hardware and firmware that manages the USB Type-C connectors on the platform.
The Cortex-A35 has a role of **UCSI OS Policy Manager** (**OPM**) to interface with the PPM, via **OpenAMP-rpmsg Virtual I2C** with the **UCSI interface**.
This application also features the **TCPP02-M18/TCPP03-M20 USB Type-C port protection** device.

For debug purpose, TRACER_EMB utility is enabled in this application. 
STM32CubeMonUCPD can be used to display USB-PD traces.
Tool can be found at the following link: [https://www.st.com/en/development-tools/stm32cubemonucpd.html](https://www.st.com/en/development-tools/stm32cubemonucpd.html).

Note: The application needs to ensure that the SysTick time base is always set to 1 millisecond
to have correct operation.

####  <b>Expected success behavior</b>
Connect STM32CubeMonUCPD on the VCP associated to your board (Cortex-M33 debug).
STM32CubeMonUCPD will be used to trace all the messages exchange between the ports partner.

#### <b>Error behaviors</b>

Errors are detected and the corresponding message is displayed on STM32CubeMonUCPD if _TRACE is enabled.

#### <b>Assumptions if any</b>

User is familiar with USB-PD 3.1 "Universal Serial BUS Power Delivery" Specification.

#### <b>Known limitations</b>
Only compilation with -Os (size) optimization option is supported for now. -Ofast (speed) option is known to make USBPD communications instable.

#### <b>FreeRTOS usage hints</b>

 - FreeRTOS uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the OS resources 
   memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.
 - For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications 
   on STM32Cube with RTOS".
 - This application uses FreeRTOS through CMSIS-RTOS2.

### <b>Keywords</b>

Connectivity, USBPD, FreeRTOS, UART/USART, USB_PD_Lib, UCPD, Type C, USBPD, UCSI

### <b>Hardware and Software environment</b>

  - This example runs on STM32MP23xx devices
  - This example has been tested with STMicroelectronics STM32MP235F-DK.
    and can be easily tailored to any other supported device and development board.

  - Connect ST-Link cable to the PC USB port to display trace data on STM32CubeMonUCPD, if _TRACE is enabled.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Run STM32CubeMonUCPD and open trace tool
 - Connect a type C device on the Type-C connector 1

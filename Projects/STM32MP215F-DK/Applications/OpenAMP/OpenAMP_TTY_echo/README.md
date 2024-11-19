## <b>OpenAMP_TTY_echo Application Description</b>

-  It demonstrates How to use OpenAMP MW + Virtual UART to create an Inter-Processor Communication channel seen as TTY device in Linux OS.
-  This project deals with CPU2 (Cortex-M33) firmware and requires Linux OS running on CPU1 (Cortex-A35).

- Following command should be done in Linux console to run the example :

        stty -onlcr -echo -F /dev/ttyRPMSG0
        cat /dev/ttyRPMSG0 &
        stty -onlcr -echo -F /dev/ttyRPMSG1
        cat /dev/ttyRPMSG1 &
        echo "Hello Virtual UART0" >/dev/ttyRPMSG0
        echo "Hello Virtual UART1" >/dev/ttyRPMSG1

You should get "Hello Virtual UART0" and "Hello Virtual UART1" in Linux console

####  <b>Expected success behavior</b>
- Blink LED3 (orange)

#### <b>Error behaviors</b>
- LED3 (orange) does not blink

### <b>Keywords</b>

OpenAMP, RPMsg, TFM, IPCC, Inter-Processor Communication

### <b>Hardware and Software environment</b>

  - This example runs on STM32MP21xx devices
  - This example has been tested with STMicroelectronics STM32MP215F-EV1.
    and can be easily tailored to any other supported device and development board.

  - Connect ST-Link cable to the PC USB port to display trace

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Connect a type C device on the Type-C connector 21

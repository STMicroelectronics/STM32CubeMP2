## <b>TFM_Protected_Storage Application Description</b>

-  It demonstrates an example of Protected Storage API (PSA) usage provided by the trusted firmware (TFM) middleware.
-  This non-secure application (TFM_Protected_Storage_CM33_NonSecure) has to be compiled after trusted firmware Secure project (TFM_Protected_Storage_CM33_trusted-firmware-m).
It contains a postbuild command which create a single binary containing NonSecure and secure FW with OPTEE image header.
-  IDE files for this secure project (TFM_Protected_Storage_CM33_trusted-firmware-m)are also present and relies on CMAKE build process not integrated inside IDE.
Plese, check in main Release Note the tools prerequisites.


####  <b>Expected success behavior</b>
- Blink LED3 (orange) led every 1s
- Print message in UART console 

        [INF] welcome to stm32mp257f eval
        [INF] Beginning TF-M provisioning
        [WRN] TFM_DUMMY_PROVISIONING is not suitable for production! This device is NOT SECURE
        [Sec Thread] Secure image initializing!
        TF-M isolation level is: 0x00000002
        Booting TF-M v1.7.0-stm32mp25-r3
        Creating an empty ITS flash layout.
        [INF][Crypto] Provisioning entropy seed... complete.
        **** TFM_Protected_Storage M33 NonSecure ****
        STM32Cube FW version: v0.2.0
        Protected Storage sample started.
        PSA Protected Storage API Version 1.0
        Writing data to UID1: WRITE_DATA_1
        Info on data stored in UID1:
        - Size: 13
        - Flags: 0x 0
        Read and compare data stored in UID1
        Data stored in UID1: WRITE_DATA_1
        Overwriting data stored in UID1: WRITE_DATA_2
        Writing data to UID2 with overwrite protection: WRITE_DATA_1
        Attempting to write 'WRITE_DATA_2' to UID2
        Got expected error (PSA_ERROR_NOT_PERMITTED) when writing to protected UID
        Removing UID1
        **** End of application ****

#### <b>Error behaviors</b>
- Print message in UART console 
- Blink LED3 (orange) led every 100ms


#### <b>Assumptions if any</b>

User is familiar with security and trusted firmware (TFM) middleware.

#### <b>Known limitations</b>


### <b>Keywords</b>

Security, TFM, Secure Storage, PSA, SNOR

### <b>Hardware and Software environment</b>

  - As a prerequisites, trusted-firmware-m source code shall be installed under "Middlewares/Third_Party" directory
  with the following path name "Middlewares/Third_Party/trusted-firmware-m" - Please make sure that you stick to the right
  trusted-firmware-m version as described in the overall STM32 MPU release note.
  - This example runs on STM32MP25xx devices
  - This example has been tested with STMicroelectronics STM32MP257F-EV1.
    and can be easily tailored to any other supported device and development board.

  - Connect ST-Link cable to the PC USB port to display trace

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Connect a type C device on the Type-C connector 21

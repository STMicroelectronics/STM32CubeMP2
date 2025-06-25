## <b>TFM_Crypto Application Description</b>

-  It demonstrates an example of Crypto APIs (PSA AEAD) usage provided by the trusted firmware (TFM) middleware.
-  This non-secure application (TFM_Crypto_CM33_NonSecure) has to be compiled 
   after trusted firmware secure project (TFM_Crypto_CM33_trusted-firmware-m).
   It contains a postbuild command which create a single binary containing NonSecure and secure FW with OPTEE image header.
-  IDE files for this secure project (TFM_Crypto_CM33_trusted-firmware-m) are also present and
   relies on CMAKE build process not integrated inside IDE.
   Plese, check in main Release Notes for the tools prerequisites.


####  <b>Expected success behavior</b>
- Blink LED3 (orange) every 1 second
- Print message in UART console :
        [INF] welcome to TF-M: v2.1.0-stm32mp-r1-rc4
        [INF] board: stm32mp215f dk
        [INF] dts: stm32mp215f-dk-ca35tdcid-ostl-s.dts
        Booting TF-M v2.1.0
        [INF] Beginning provisioning
        [INF] DUMMY_PROVISIONING is not suitable for production!
        [INF] This device is NOT SECURE
        [WAR] This device was provisioned with dummy keys.
        [WAR] This device is NOT SECURE
        [Sec Thread] Secure image initializing!
        TF-M isolation level is: 0x00000002
        [INF][Crypto] Provision entropy seed...
        [INF][Crypto] Provision entropy seed... complete.
        [DBG][Crypto] Init Mbed TLS 3.6.2...
        [DBG][Crypto] Init Mbed TLS 3.6.2... complete.

        ***** TFM_Crypto M33 NonSecure *****
        STM32Cube FW version: v1.2.0
        PSA Crypto example started.
        PSA Crypto API Version 1.0
        Performing AEAD encryption (Single-Shot)
        Performing AEAD decryption (Single-Shot)
         --> Decrypted data is same as the original data (Single-Shot)
        Performing AEAD Encryption (Multi-Part)
         --> Single-Shot encrypted data matches with Multi-Part encrypted data
        Performing AEAD Decryption (Multi-Part)
         --> Decrypted data matches with plain text (Multi-Part)
        PSA Crypto example executed successfully.
        *****    End of Application    *****


#### <b>Error behaviors</b>
- LED3 (orange) permanent on (no blinking)
- Print message in UART console :
        Error : Something went wrong


#### <b>Assumptions if any</b>
User is familiar with security and trusted firmware (TFM) middleware.

#### <b>Known limitations</b>


### <b>Keywords</b>
Security, TFM, Crypto, PSA, AEAD

### <b>Hardware and Software environment</b>

  - As a prerequisites, trusted-firmware-m source code shall be installed under "Middlewares/Third_Party" directory
    with the following path name "Middlewares/Third_Party/trusted-firmware-m"
  - Please make sure that you stick to the right
    trusted-firmware-m version as described in the overall STM32 MPU release note.
  - This example runs on STM32MP21xx devices.
  - This example has been tested with STMicroelectronics STM32MP215F-DK
    and can be easily tailored to any other supported device and development board.
  - Connect ST-Link cable to the PC USB port to display trace.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application


# SSL Server application

<!--
******************** (C) COPYRIGHT 2026 STMicroelectronics *******************
@file    mbedTLS/SSL_Server/readme.md
@author  MCD Application Team
@brief   Description of SSL Server application.
******************************************************************************

Copyright (c) 2026 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.
******************************************************************************
-->

## Application Description

This application describes how  to run an SSL server application based on mbedTLS
crypto library and LwIP TCP/IP stack.

At the beginning of the main program the `HAL_Init()` function is called to reset
all the peripherals and initialize the Flash interface.
The HAL initializes the TIM6 to generate an interrupt each 1ms, it will be used as time base for the HAL drivers.
This application uses FreeRTOS, the RTOS initializes the systick to generate an interrupt each 1ms.
The systick is then used for FreeRTOS time base.
The `SystemClock_Config()` function is used to configure the system clocks for STM32MP25xx devices (for example, STM32MP257F).
In this application, the MCU high-speed clock (ICN_HS_MCU) is configured to 400 MHz (PLL4 at 1200 MHz divided by 3).
The Ethernet clock is configured to 125 MHz (PLL6 at 500 MHz divided by 4).

### Scenario 1:

In this scenario, the server (STM32MP257F-EV1):

- connects to local network either through DHCP or static IP address.
- use a web browser on a PC to connect to the URL `https://<server_board_ip_address>:4433`
- once the connection is established, the server sends a message to the client
- the message is displayed on the browser webpage

### Scenario 2:

In this second scenario, 2 boards can be used to test the application

- connect the boards to the local network either through DHCP or static IP address.
- a second STM32MP257F-EV1 board running the "SSL_Server" application
  connects to the URL `https://<server_board_ip_address>:4433`
- the client sends an encrypted message to the server.
- the server board responds with the suitable message.
- the received message is displayed on the UART.

Application logs are displayed on the UART.

In case of success, the green LED is on.
In case of error, the red LED is on.

> **Note:** Care must be taken when using `HAL_Delay()`, this function provides accurate delay (in milliseconds)
> based on variable incremented in HAL time base ISR. This implies that if `HAL_Delay()` is called from
> a peripheral ISR process, then the HAL time base interrupt must have higher priority (numerically lower)
> than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
> To change the HAL time base interrupt priority you have to use `HAL_NVIC_SetPriority()` function.

> **Note:** The application needs to ensure that the HAL time base is always set to 1 millisecond
> to have correct HAL operation.

> **Note:** The message sent by the server depends on the cipher suite agreed with the client.
> It is similar to:

```text
" mbed TLS Test Server
  Successful connection using: TLS-ECDHE-RSA-WITH-AES-256-GCM-SHA384"
```

For more details about this application, refer to UM1723 "STM32Cube mbedTLS application".

## Keywords

Security, mbedTLS, HASH, RNG, CRYP, SSL Server, Crypto, TCP/IP, LwIP, AES 128,
AES 192, AES 256, Triple DES, MD5, SHA-1

## Directory contents

- `CA35/Secure/Core/Inc/*.h` Application headers (HAL, BSP, mbedTLS, LwIP)
- `CA35/Secure/Core/Src/main.c` Main program
- `CA35/Secure/FreeRTOS/App/app_freertos.c` FreeRTOS application hooks/tasks
- `CA35/Secure/LwIP/App/*` LwIP application files
- `CA35/Secure/LwIP/Target/*` LwIP Ethernet interface glue
- `CA35/Secure/SSL/ssl_server.c` SSL server main thread
- `CA35/Secure/SSL/hardware_rng.c` RNG/entropy integration
- `CM33/NonSecure/Core/Inc/*.h` Application headers (HAL, BSP, mbedTLS, LwIP)
- `CM33/NonSecure/Core/Src/main.c` Main program
- `CM33/NonSecure/FreeRTOS/App/app_freertos.c` FreeRTOS application hooks/tasks
- `CM33/NonSecure/LwIP/App/*` LwIP application files
- `CM33/NonSecure/LwIP/Target/*` LwIP Ethernet interface glue
- `CM33/NonSecure/SSL/ssl_server.c` SSL server main thread
- `CM33/NonSecure/SSL/hardware_rng.c` RNG/entropy integration

## Hardware and Software environment

- This application runs on STM32MP25xx devices (for example, STM32MP257F-EV1).

- This application has been tested with two scenarios.
  - Using a Windows PC equipped with a web browser as a client.
  - using two STM32MP257F-EV1 boards: the first as server, the second as client.

- Scenario 1:
  - Remote PC Set-up
    - Make sure that the PC is in the same local network as the board.
    - The PC has a web browser that supports TLS and secure connections (IE > 10, Chrome > 52, Firefox > 41)

- Scenario 2:
  - a second STM32MP257F-EV1 running the "mbedTLS/SSL_Server" application

- STM32MP257F-EV1 setup
  - Connect the STM32MP257F-EV1 to your local network (through a straight Ethernet cable).

## Prerequisite external DT modifications

When running the CM33 non-secure application with the CA35-TDCID M33-examples
configuration, assign the hardware RNG to privileged non-secure CID2 accesses
for the M33 application. OP-TEE then uses its software PRNG instead of this
hardware RNG. The application's mbedTLS entropy source remains the hardware RNG.

> **Note:** This configuration is for demonstration only. With the hardware RNG
> unavailable to OP-TEE, the software PRNG falls back to a predictable time-based
> seed. It does not provide production-quality entropy for OP-TEE. Keep these
> changes as local demonstration overrides, not as the mainline external-DT
> configuration. A production system must provide a trusted entropy source to
> OP-TEE.

In the external DT sources, under `stm32mp2/a35-td/`:

1. Set the RNG node to disabled in the OP-TEE board file:
  `optee/stm32mp257f-ev1-ca35tdcid-ostl-m33-examples.dts`.

  ```dts
  &rng {
     status = "disabled";
  };
  ```

2. Replace the existing RNG entry in the `&rifsc` node's `st,protreg` property in
  `optee/stm32mp257f-ev1-ca35tdcid-ostl-m33-examples-rif.dtsi` with:

  ```dts
  RIFPROT(STM32MP25_RIFSC_RNG_ID, RIF_UNUSED, RIF_LOCK,
        RIF_NSEC, RIF_PRIV, RIF_CID2, RIF_SEM_DIS, RIF_CFEN)
  ```

The disabled node prevents OP-TEE from probing the RNG. The RIF entry
allows the M33's privileged non-secure CID2 accesses; it does not grant access
to Linux/CID1. Apply these overrides only to the M33-examples board configuration.

After applying these external DT changes, rebuild OP-TEE for its board DT and
RIF changes.
As part of this OP-TEE rebuild, append the following options to the existing
build options:

```makefile
CFG_HWRNG_PTA=n \
CFG_WITH_SOFTWARE_PRNG=y \
CFG_WITH_TRNG=n \
CFG_STM32_PKA=n \
CFG_STM32_SAES=n
```

These options disable OP-TEE's hardware RNG service and select the software PRNG
needed by its secure services, including remoteproc TA initialization. PKA and
SAES are disabled because they depend on RNG support. Keep `CFG_STM32_RNG=y` for
the platform PRNG initialization handler. With the RNG node disabled in the DT,
this handler uses fallback initialization without hardware RNG entropy. The RNG
IP is not used to seed OP-TEE's PRNG in this configuration. Keep
`CFG_STM32MP_PROFILE=secure_and_system_services` for remoteproc support.
Disabling the DT node alone is not sufficient to select the software PRNG.

Update only the board's Firmware Image Package (FIP) with the rebuilt OP-TEE images
before running the application. Follow the OpenSTLinux procedure in
[How to configure TF-A FIP](https://wiki.st.com/stm32mpu/wiki/How_to_configure_TF-A_FIP).

For more information, refer to the STM32 MPU Wiki:

- [OP-TEE configuration switches](https://wiki.st.com/stm32mpu/wiki/OP-TEE_configuration_switches)
- [RNG device tree configuration](https://wiki.st.com/stm32mpu/wiki/RNG_device_tree_configuration)
- [RIFSC device tree configuration](https://wiki.st.com/stm32mpu/wiki/RIFSC_device_tree_configuration)

## How to use it ?

In order to make the program work, you must do the following :

- Open your preferred toolchain
- Rebuild all files and load your image into target memory
- Run the application.

**Note:**

1. Make sure that the url passed to the client is `https://Server_board_address:4433`

2. The server is using a non-trusted certificate; thus some clients (in particular web browsers) may show errors during connection.
   Please bypass that error to get the application running.

3. If using two boards for testing and the DHCP is enabled then the "SSL_Server" needs to be
   run first, to get the board IP Address and use it in the "SSL_Server" application.

4. If the board is connected to a LAN, ensure that it has a unique MAC address. The MAC address can be defined in the file
   "Core/Inc/stm32mp2xx_hal_conf.h"

5. If needed, adjust the personalization string (pers) in "SSL/ssl_server.c".
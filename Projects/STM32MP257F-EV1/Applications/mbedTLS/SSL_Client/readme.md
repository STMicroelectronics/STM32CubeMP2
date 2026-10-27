# SSL Client application

<!--
******************** (C) COPYRIGHT 2026 STMicroelectronics *******************
@file    mbedTLS/SSL_Client/readme.md
@author  MCD Application Team
@brief   Description of SSL Client application.
******************************************************************************

Copyright (c) 2026 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.
******************************************************************************
-->

## Application Description

This application describes how  to run an SSL client application based on mbedTLS
crypto library and LwIP TCP/IP stack.

At the beginning of the main program, `HAL_Init()` is called to reset the
peripherals and initialize the HAL time base. When the board runs in developer
boot mode, `SystemClock_Config()` configures the clocks for the M33 execution
context. Otherwise the example reuses the system clock configuration already
provided by the platform startup. The application then initializes LED support,
the COM1 UART used for logs, the Ethernet interface, the hardware RNG, and the
FreeRTOS kernel before creating the SSL client task.

The SSL client task performs the following sequence:

- initializes the entropy and CTR-DRBG contexts
- loads the embedded mbedTLS test CA certificate
- connects to `SERVER_NAME:SERVER_PORT` defined in
  `CM33/NonSecure/Core/Inc/main.h`
- configures a TLS client session with `MBEDTLS_SSL_VERIFY_OPTIONAL`
- performs the TLS handshake
- sends the HTTP request "GET / HTTP/1.0"
- prints the received response on the UART console

Then the application can run one of the following scenarios:

### Scenario 1:

In this scenario, the client (STM32MP257F-EV1):

- connects to local network either through DHCP or static IP address.
- establishes a secured connected to the Host PC Server.
- sends an encrypted message to the server.
- receives an answser from the server.

### Scenario 2:

In this second scenario, the client (STM32MP257F-EV1):

- connects to local network either through DHCP or static IP address.
- establishes a secured connected to a second STM32MP257F-EV1 board running
  the "SSL_Sever" application.
- sends an encrypted message to the second board.
- receives an answser from the server board.

Application logs are displayed on the UART.

In case of success, the green LED toggles.
In case of error, the red LED toggles.

> **Note:** Care must be taken when using `HAL_Delay()`, this function provides an
> accurate delay in milliseconds based on a variable incremented in the HAL
> time-base ISR. If `HAL_Delay()` is called from a peripheral ISR, the HAL
> time-base interrupt must have a higher priority (numerically lower) than
> the peripheral interrupt. Otherwise the caller ISR remains blocked.

> **Note:** The application needs the HAL time base to remain configured to
> 1 millisecond for correct HAL operation.

> **Note:** The example loads the embedded mbedTLS test CA certificate and sets the
> TLS hostname to "localhost" in `ssl_client.c`. If you use another server
> certificate or another hostname, update the TLS configuration accordingly.

For more details about this application, refer to UM1723 "STM32Cube mbedTLS
application".

## Keywords

Security, mbedTLS, HASH, RNG, CRYP, SSL Client, Crypto, TCP/IP, LwIP, AES 128,
AES 192, AES 256, Triple DES, MD5, SHA-1

## Directory contents

- `CM33/NonSecure/Core/Inc/main.h` Main program header file
- `CM33/NonSecure/Core/Inc/mbedtls_config.h` mbedTLS configuration options
- `CM33/NonSecure/Core/Inc/FreeRTOSConfig.h` FreeRTOS configuration options
- `CM33/NonSecure/Core/Inc/lwipopts.h` LwIP configuration options
- `CM33/NonSecure/Core/Inc/stm32mp2xx_hal_conf.h` HAL library configuration file
- `CM33/NonSecure/Core/Inc/stm32mp2xx_it.h` Interrupt handlers header file
- `CM33/NonSecure/Core/Src/main.c` Main program
- `CM33/NonSecure/Core/Src/app_freertos.c` FreeRTOS object creation
- `CM33/NonSecure/Core/Src/app_ethernet.c` Ethernet IP handling
- `CM33/NonSecure/Core/Src/lwip.c` LwIP stack initialization
- `CM33/NonSecure/Core/Src/ethernetif.c` Ethernet driver glue layer
- `CM33/NonSecure/Core/Src/net_sockets.c` mbedTLS socket port based on LwIP
- `CM33/NonSecure/Core/Src/hardware_rng.c` RNG support for entropy
- `CM33/NonSecure/Core/Src/ssl_client.c` SSL client task
- `CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c` HAL MSP module
- `CM33/NonSecure/Core/Src/stm32mp2xx_hal_timebase_tim.c` HAL time base functions
- `CM33/NonSecure/Core/Src/stm32mp2xx_it.c` Interrupt handlers

## Hardware and Software environment

- This application runs on STM32MP257F-EV1 boards.

- This application has been tested with two scenarios.
  - Using a Host PC as server, running a Fedora 23 operating System or similar.
    - Using a Windows PC as server, running Windows 7 operating system or later.
  - using two stm32mp257f-ev1 boards the first as server, the second as client.

- Scenario 1:
  - Remote PC Set-up
    - Make sure that the PC is in the same local network as the baord.
    - Using a Linux Machine
      - Download the mebedTLS source code from (<https://tls.mbed.org/download/start/mbedtls-2.4.0-apache.tgz>)
      - build the mbedTLS package following the instructions in the <https://github.com/ARMmbed/mbedtls#make>
      - Once done, run the server application "programs/ssl/ssl_server".
    - Using a Windows Machine
      - Follow the instructions in the file "Firmware\Utilities\PC_Software\ssl_server\readme.txt" to run the ssl_server application.
    - the "ssl_server" will wait for remote connections.

- Scenario 2:
  - a second STM32MP257F-EV1 running the "mdedTLS/SSL_Server" application

- STM32MP257F-EV1 Set-up
  - Connect STM32MP257F-EV1 to your local network (through a straight ethernet cable).

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

In order to make the program work, you must do the following:

- Open the CM33 non-secure project in your preferred toolchain.
- Edit `CM33/NonSecure/Core/Inc/main.h` and update `SERVER_NAME` if the server IP
  address differs from the default value.
- Rebuild all files and load the image into target memory.
- Connect the board to the local network.
- Run the application and monitor the UART console.

**Note:**

1. Please ensure that the remote PC IP address is the same IP address
   as the one defined in `main.h` in the "SERVER_NAME", and the port number
   used for the connection is the same as the one defined in "SERVER_PORT"

2. Notice that the PC firewall may prevent the application from running correctly, by blocking
   the communication via the "SERVER_PORT".

3. If using two boards for testing and the DHCP is enabled then the "SSL_Server" needs to be
   run first, to get the board IP Address and use it in the "SSL_Client" application.

4. If the board is connected to a LAN, ensure that it has a unique MAC address. The MAC address
   can be defined in the `MX_ETH_Init()` function in the `main.c` file.

5. If using two boards for testing then the pers value in the "Src/ssl_client.c" should be same as pers value in the "Src/ssl_server.c
   file.
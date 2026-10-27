# OpenAMP_TTY_echo Application

---

## Application Description

The OpenAMP_TTY_echo application demonstrates Inter-Processor Communication (IPC) using the
**OpenAMP middleware** over IPCC1. Two independent CubeIDE projects are provided under this
folder, each targeting a different processor:

### CM33 NonSecure

- **CM33** acts as the **REMOTE** processor. It runs as a bare-metal firmware, loaded and started
  by Linux on CA35 via the RemoteProc framework.
- **CA35** acts as the **HOST** (OpenAMP RPROC master). Linux controls the CM33 lifecycle and
  exchanges messages through the RPMsg/VirtIO transport over IPCC1.
- The CM33 creates two Virtual UART channels (**VirtUART0** → `/dev/ttyRPMSG0`,
  **VirtUART1** → `/dev/ttyRPMSG1`) and echoes back every message it receives.
- **LED3 (orange)** toggles on each successfully received message.

### CA35 Secure

- This project targets the **M33TD** use case: CM33 is the Trusted Domain master; CA35 runs as a
  bare-metal coprocessor loaded and started by **StarterApp_M33TD_BM** on CM33 via RemoteProc.
- **CA35** acts as the **RPMsg HOST** (`RPMSG_HOST` role), creating a fixed endpoint
  (name: `"bm_echo"`, address: `0x5C`) over IPCC1 and sending `"Hello N from A35"` every 3 s.
- **CM33** (StarterApp_M33TD_BM) acts as the **RPMsg REMOTE** and echoes each message back.

---

## Expected Behavior

### CM33 NonSecure

- LED3 (orange) toggles on each message received from Linux.
- Data written to `/dev/ttyRPMSG0` and `/dev/ttyRPMSG1` is echoed back and visible in the Linux
  console.

### CA35 Secure

- A35 UART prints `[OpenAMP] TX` and `[OpenAMP] RX echo` messages alternately every 3 seconds.
- Expected serial terminal output:

  ```
  **** OpenAMP_TTY_echo application - CA35 Secure ****
  ***********************************************************
  *     CA35 Frequency : 1200000 KHz
  *     CA35 FCK_SRC   : PLL1
  ***********************************************************
  [OpenAMP] TX [32 bytes]: Hello 0 from A35
  [OpenAMP] RX echo [32 bytes]: Hello 0 from A35
  [OpenAMP] TX [32 bytes]: Hello 1 from A35
  [OpenAMP] RX echo [32 bytes]: Hello 1 from A35
  ```

---

## Error Behaviors

### CM33 NonSecure

- LED3 (orange) does not blink: CM33 firmware did not start or VirtUART initialisation failed.
  Check that the signed binary is correctly placed and RemoteProc state is `running`.

### CA35 Secure

- No `[OpenAMP] RX echo` messages: verify that **StarterApp_M33TD_BM** is running on CM33.
- `error while initializing OpenAMP`: check IPCC1 and shared memory configuration.

---

## Keywords

OpenAMP, RPMsg, VirtUART, IPCC, Inter-Processor Communication, RemoteProc, bare-metal, CM33,
CA35, Linux

---

## Directory Contents

### CM33 NonSecure

```
CM33/NonSecure/
├── Core/
│   ├── Inc/
│   │   ├── main.h                        Main application header
│   │   ├── stm32mp215f_disco_conf.h      STM32MP215F-DK board configuration
│   │   ├── stm32mp2xx_hal_conf.h         HAL configuration
│   │   └── stm32mp2xx_it.h              Interrupt handlers header
│   └── Src/
│       ├── main.c                        Main application (VirtUART init + echo loop)
│       ├── stm32mp2xx_hal_msp.c          HAL MSP initialisation
│       └── stm32mp2xx_it.c              Interrupt handlers
└── OPENAMP/
    ├── mbox_ipcc.c / .h                  IPCC1 mailbox implementation
    ├── openamp.c / .h                    OpenAMP wrapper (init, deinit, poll)
    ├── openamp_conf.h                    OpenAMP role and transport configuration
    ├── openamp_log.c / .h                Trace/log helpers
    └── rsc_table.c / .h                  Resource table (shared memory layout)
```

### CA35 Secure

```
CA35/Secure/
├── Inc/
│   ├── main.h                            Main header
│   ├── stm32mp215f_disco_conf.h          STM32MP215F-DK board configuration
│   ├── stm32mp2xx_hal_conf.h             HAL configuration
│   ├── stm32mp2xx_it.h                   Interrupt handlers header
│   ├── stm32mp2xx_mmu.h                  MMU definitions
│   └── stm32mp2xx_mmu_conf.h             MMU region configuration (IPC shmem at 0x80A00000)
├── OpenAMP/
│   ├── Inc/
│   │   ├── mbox_ipcc.h                   IPCC mailbox interface header
│   │   ├── openamp.h                     OpenAMP wrapper header
│   │   ├── openamp_conf.h                OpenAMP configuration (HOST role, IPCC mailbox)
│   │   ├── openamp_log.h                 OpenAMP log interface
│   │   └── rsc_table.h                   Resource table header
│   └── Src/
│       ├── mbox_ipcc.c                   IPCC mailbox implementation
│       ├── openamp.c                     OpenAMP wrapper implementation
│       └── rsc_table.c                   Resource table implementation
└── Src/
    ├── main.c                            Main program
    ├── mmu_stm32mp2xx.c                  MMU translation table
    ├── startup_stm32mp21xx_a35.c         A35 AArch32 start-up source
    ├── stm32mp2xx_hal_msp.c              HAL MSP initialisation
    ├── stm32mp2xx_it.c                   Interrupt handlers
    ├── syscalls.c                        System calls (newlib stubs)
    └── system_stm32mp2xx_a35.c          System initialisation
```

---

## Hardware and Software Environment

- This application runs on **STM32MP21xxxx** devices.
- Tested with **STMicroelectronics STM32MP215F-DK** board.
- **CM33 project**: Linux must be running on CA35 with RemoteProc and RPMsg support enabled in
  the kernel. Connect an ST-Link cable to the PC USB port to observe CM33 trace output.
- **CA35 project** (M33TD): **StarterApp_M33TD_BM** runs on CM33 as Trusted Domain master,
  manages CA35 via RemoteProc, and acts as the RPMsg remote endpoint. Refer to
  `Projects/STM32MP215F-DK/Demonstrations/StarterApp_M33TD_BM`. Connect an ST-Link cable to
  the PC USB port to display A35 UART trace (USART2, 115200 baud, 8N1).

---

## To Build with STM32CubeIDE

### Project Structure

```
STM32CubeIDE/
├── .project                              Workspace-level project (import this)
├── CM33/
│   ├── .project                          CM33 aggregate project
│   └── NonSecure/
│       ├── .project                      OpenAMP_TTY_echo_CM33_NonSecure project
│       └── .cproject                     Build configurations
└── CA35/
    ├── .project                          CA35 aggregate project
    └── Secure/
        ├── .project                      OpenAMP_TTY_echo_CA35_Secure project
        └── .cproject                     Build configurations
```

> **Note**: Refer to the
> [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/How_to_create_an_M33-TD_boot_project_using_STM32CubeIDE)
> for instructions on importing a project into STM32CubeIDE.

### Import

1. Open STM32CubeIDE.
2. `File → Import → General → Existing Projects into Workspace`.
3. Browse to `STM32CubeIDE/` and import all discovered projects.

### CM33 NonSecure Build Configurations

| Configuration | Description |
|---------------|-------------|
| `CA35TDCID_m33_ns` | Debug build — produces stripped ELF only |
| `CA35TDCID_m33_ns_sign` | **Production build** — runs `sign_rproc_fw.py` post-build to produce the signed binary |

To build:
- Right-click on `OpenAMP_TTY_echo_CM33_NonSecure` → `Build Configurations → Set Active` →
  select `CA35TDCID_m33_ns_sign`.
- Right-click → `Build Project`.

### CA35 Secure Build Configuration

| Configuration | Description |
|---------------|-------------|
| `CM33TDCID_a35_aarch32_s_sign` | Signs and links `OpenAMP_TTY_echo.bin` for programming via StarterApp |

To build:
- Right-click on `OpenAMP_TTY_echo_CA35_Secure` → `Build Configurations → Set Active` →
  select `CM33TDCID_a35_aarch32_s_sign`.
- Right-click → `Build Project`.

---

## How to Deploy and Run

### CM33 — Linux RemoteProc

1. Copy `OpenAMP_TTY_echo_CM33_NonSecure_sign.bin` to the Linux filesystem on the target board:
   ```bash
   cp OpenAMP_TTY_echo_CM33_NonSecure_sign.bin /lib/firmware/
   ```

2. Load and start the CM33 firmware:
   ```bash
   echo OpenAMP_TTY_echo_CM33_NonSecure_sign.bin > /sys/class/remoteproc/remoteproc0/firmware
   echo start > /sys/class/remoteproc/remoteproc0/state
   ```

3. Configure the VirtUART channels and send test messages:
   ```bash
   stty -onlcr -echo -F /dev/ttyRPMSG0
   cat /dev/ttyRPMSG0 &
   stty -onlcr -echo -F /dev/ttyRPMSG1
   cat /dev/ttyRPMSG1 &
   echo "Hello Virtual UART0" > /dev/ttyRPMSG0
   echo "Hello Virtual UART1" > /dev/ttyRPMSG1
   ```

4. Expected output in the Linux console:
   ```
   Hello Virtual UART0
   Hello Virtual UART1
   ```

5. To stop the CM33 firmware:
   ```bash
   echo stop > /sys/class/remoteproc/remoteproc0/state
   ```

### CA35 — M33TD Bare-Metal

1. Build the CA35 Secure project in STM32CubeIDE.

2. Follow `Projects/STM32MP215F-DK/Demonstrations/StarterApp_M33TD_BM/README.md` to compile the
   remaining binaries. The CA35 application binary needs to be signed — while building TFM Secure
   in StarterApp_M33TD_BM, pass the absolute path to the CA35 application binary using:
   ```
   -DSTM32_CA35_FW=<absolute_path_to_CA35_binary>
   ```
   The signed CA35 firmware will be generated in the `trusted-firmware-m/config_default/bin`
   folder.

3. Program the signed CA35 binary along with all binaries generated in StarterApp_M33TD_BM
   and reset the board.

4. Open a serial terminal on the A35 UART (**USART2, 115200 baud, 8N1**) to observe the echo
   output.

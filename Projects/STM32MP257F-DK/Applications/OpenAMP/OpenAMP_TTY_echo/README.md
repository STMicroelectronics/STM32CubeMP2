# OpenAMP_TTY_echo Application

---

## Application Description

The OpenAMP_TTY_echo application demonstrates Inter-Processor Communication (IPC) using the
**OpenAMP middleware** over IPCC1. A single CubeIDE project is provided targeting the CM33
processor:

### CM33 NonSecure

- **CM33** acts as the **REMOTE** processor. It runs as a bare-metal firmware, loaded and started
  by Linux on CA35 via the RemoteProc framework.
- **CA35** acts as the **HOST** (OpenAMP RPROC master). Linux controls the CM33 lifecycle and
  exchanges messages through the RPMsg/VirtIO transport over IPCC1.
- The CM33 creates two Virtual UART channels (**VirtUART0** → `/dev/ttyRPMSG0`,
  **VirtUART1** → `/dev/ttyRPMSG1`) and echoes back every message it receives.
- **LED3 (orange)** toggles on each successfully received message.

---

## Expected Behavior

### CM33 NonSecure

- LED3 (orange) toggles on each message received from Linux.
- Data written to `/dev/ttyRPMSG0` and `/dev/ttyRPMSG1` is echoed back and visible in the Linux
  console.

---

## Error Behaviors

### CM33 NonSecure

- LED3 (orange) does not blink: CM33 firmware did not start or VirtUART initialisation failed.
  Check that the signed binary is correctly placed and RemoteProc state is `running`.

---

## Keywords

OpenAMP, RPMsg, VirtUART, IPCC, Inter-Processor Communication, RemoteProc, bare-metal, CM33,
Linux

---

## Directory Contents

### CM33 NonSecure

```
CM33/NonSecure/
├── Core/
│   ├── Inc/
│   │   ├── main.h                        Main application header
│   │   ├── stm32mp257f_disco_conf.h      STM32MP257F-DK board configuration
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

---

## Hardware and Software Environment

- This application runs on **STM32MP25xxxx** devices.
- Tested with **STMicroelectronics STM32MP257F-DK** board.
- Linux must be running on CA35 with RemoteProc and RPMsg support enabled in the kernel.
- Connect an ST-Link cable to the PC USB port to observe CM33 trace output.

---

## To Build with STM32CubeIDE

### Project Structure

```
STM32CubeIDE/
├── .project                              Workspace-level project (import this)
└── CM33/
    ├── .project                          CM33 aggregate project
    └── NonSecure/
        ├── .project                      OpenAMP_TTY_echo_CM33_NonSecure project
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
|---------------|--------------|
| `CA35TDCID_m33_ns` | Debug build — produces stripped ELF only |
| `CA35TDCID_m33_ns_sign` | **Production build** — runs `sign_rproc_fw.py` post-build to produce the signed binary |

To build:
- Right-click on `OpenAMP_TTY_echo_CM33_NonSecure` → `Build Configurations → Set Active` →
  select `CA35TDCID_m33_ns_sign`.
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

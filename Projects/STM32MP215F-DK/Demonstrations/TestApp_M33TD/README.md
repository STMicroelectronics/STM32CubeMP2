# TestApp_M33TD Demonstration Application

---

## Application Description

The TestApp_M33TD demonstration highlights the STM32MP2 M33TDCID profile with Cortex-M33 as the primary CPU and Cortex-A35 as a managed coprocessor.

This project is built on the shared `Utilities/M33TD_NSAppCore` stack and demonstrates how to layer a project-specific validation task on top of the common utility profile. Board-specific drivers and local adaptations are implemented under `CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/`.

---

## Key Features

- **FreeRTOS Multitasking**: Runs multiple tasks in the non-secure environment.
- **Utility Stack Integration**: Uses `Utilities/M33TD_NSAppCore` for reusable bootstrap, task logic, and thin driver abstractions.
- **A35 Coprocessor Management**: Uses the RemoteProc task to manage the A35 lifecycle through TF-M secure services.
- **Low-Power Orchestration**: Includes the LowPowerMgr task in the default TestApp profile.
- **UserApp Task**: Keeps the example LED activity enabled.
- **TestApp Task**: Runs a sequence of platform tests in the NS environment.
- **Optional Display Pipeline**: `BUILD_CONFIG=FULL` enables the display path; `BUILD_CONFIG=MINIMUM` keeps the non-display profile.

---

## Purpose

This is a FreeRTOS-based multi-task application running in the NS processing environment while TF-M runs in the secure environment and provides secure services.

In the current STM32MP215F-DK TestApp profile, UserApp, LowPowerMgr, Button Monitor, and OpenAMP are enabled by default; DisplayTask is added automatically in `BUILD_CONFIG=FULL`. `NSCoreApp_Init()` starts the tasks below:

1. **NSCoreApp (Bootstrap)**:
   - Initializes the common stack and starts the enabled tasks.

2. **Logger Task**:
   - Centralized logging, with optional real-time output.

3. **UserApp Task**:
   - Keeps the example LED activity enabled.

4. **LowPowerMgr Task**:
   - Owns the low-power policy and suspend/resume sequencing used by the TestApp profile.

5. **SCMI Manager Task**:
   - Handles SCMI notifications and related TF-M forwarding.

6. **OpenAMP Task**:
   - Provides the RPMsg transport used by the project profile.

7. **RemoteProc Task**:
   - Manages A35 lifecycle control, status reporting, and recovery flows.

8. **Watchdog Monitor Task**:
   - Supervises system health and watchdog-related handling.

9. **Button Monitor Task**:
   - Monitors the USER button and dispatches project-side actions.

10. **Display Task (optional)**:
    - Added automatically when `BUILD_CONFIG=FULL` enables the display pipeline.

11. **TestApp Task**:
    - Executes the platform and example test sequence.

---

## Prerequisite Hardware & Software Environment Setup

- **Trusted Firmware-M**: Install the source code under `Middlewares/Third_Party/trusted-firmware-m`.
- **Supported Devices**: This example targets STM32MP21xx devices and has been validated on STM32MP215F-DK.
- **ST-Link Connection**: Connect the ST-Link cable to the PC USB port to display traces.

### Software Versions
- **Trusted Firmware-M (TFM)**: Refer to the [Trusted Firmware-M wiki](https://wiki.st.com/stm32mpu/wiki/Category:Trusted_Firmware-M).
- **External Device Tree (externalDT)**: See the [External Device Tree wiki](https://wiki.st.com/stm32mpu/wiki/External_device_tree).
- **STM32CubeIDE**: Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/).

---

## Artifact flow (build -> sign -> deploy)

<pre>
+----------------+
| TF-M build     |
+----------------+
  +-> bl2*.stm32
   +-> ddr_phy*_Signed.bin
  +-> tfm_s.bin

+---------------------+
| CM33-NS app build   |
+---------------------+
   +-> ${PROJECT_NAME}.bin

+------------------------------+
| postbuild (assemble + sign)  |
+------------------------------+
   | inputs: ${PROJECT_NAME}.bin + tfm_s.bin
   +-> tfm-*_s_ns_Signed.bin

Deploy into OSTL image tree
    bl2*.stm32            -> .../images/stm32mp2-m33td/arm-trusted-firmware-m/bl2
    ddr_phy*_Signed.bin   -> .../images/stm32mp2-m33td/m33-firmware
    tfm-*_s_ns_Signed.bin -> .../images/stm32mp2-m33td/m33-firmware
</pre>

---

## To Build CMake Project for M33TDCID (Through Command Line)

### Compilation Instructions
- Ensure an ARM cross-compiler toolchain with `arm-none-eabi-gcc` is available in `PATH`.
- When using `-G"Unix Makefiles"` on Windows, also ensure `make` is available in `PATH`.

---

## Build Procedure

### Secure Build (TFM)
1. Navigate to `Firmware/Middlewares/Third_Party/trusted-firmware-m`.
2. For SD card development mode, execute the following command:
   ```bash
   cmake -B config_default -G"Unix Makefiles" -DTFM_PLATFORM=stm/stm32mp215f_dk -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake -DSTM32_BOOT_DEV=sdmmc1 -DTFM_PROFILE=profile_medium -DSTM32_M33TDCID=ON -DCMAKE_BUILD_TYPE=Relwithdebinfo -DNS=OFF -DDTS_EXT_DIR=<EXT_DT_DIR> -DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts -DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts -DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-ns.dts
   ```
3. Build the project:
   ```bash
   cmake --build config_default -- install
   ```

**Note**: The external DT repository is placed in the `Utilities` directory.

---

### Non-Secure M33 Firmware Build (TestApp_M33TD Application)
1. Navigate to `Firmware/Projects/STM32MP215F-DK/Demonstrations/TestApp_M33TD`.
2. Run the following command:
   ```bash
   cmake -G"Unix Makefiles" -B build -DTFM_BUILD_DIR=<TFM_BUILD_DIRECTORY> -DENABLE_AUTO_TEST=ON
   ```
   - If `TFM_BUILD_DIR` is not specified, the default path `Firmware/Middlewares/Third_Party/trusted-firmware-m/config_default` is used.
   - Current project CMake options:
     - `-DBUILD_CONFIG=FULL|MINIMUM` (default: `FULL`)
     - `-DBOOT_SPLASHSCREEN=DYNAMIC|STATIC` (default: `DYNAMIC`, meaningful with `BUILD_CONFIG=FULL`)
     - `-DREMOTE_PROC_AUTO_START=ON|OFF` (default: `ON`)
     - `-DLOW_POWER_DEFAULT_POLICY_ENABLE=ON|OFF` (default: `OFF`)
     - `-DREALTIME_DEBUG_LOG_ENABLED=ON|OFF` (default: `OFF`)
     - `-DFAULT_EXCEPTION_ENABLE=ON|OFF` (default: `ON`)
     - `-DFAULT_EXCEPTION_BACKTRACE_ENABLE=ON|OFF` (default: `ON`)
     - `-DENABLE_AUTO_TEST=ON|OFF` (default: `ON`)
3. Build the project:
   ```bash
   make -C build all
   ```

---

## To Build Native STM32CubeIDE Project

### Project Structure
```
TestApp_M33TD
├── TestApp_M33TD_CM33
│   ├── TestApp_M33TD_CM33_NonSecure
│   └── TestApp_M33TD_CM33_trusted-firmware-m
```

**Note**: Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/How_to_create_an_M33-TD_boot_project_using_STM32CubeIDE#) for project import guidance.

### Build Procedure

#### Secure TFM Firmware Build
1. Configure the TF-M CMake project with the same platform and DTS values used by the command-line flow.
2. Run `CMake Configure` on `TestApp_M33TD_CM33_trusted-firmware-m`.
3. Build the TF-M CMake project.

#### Non-Secure STM32CubeIDE Project Build
1. Select the matching NonSecure build configuration.
2. Build `TestApp_M33TD_CM33_NonSecure`.

---

## Output

- The generated binaries will be located in the `bin` folder:
```
cd bin/
```
   - `TestApp_M33TD_CM33_NonSecure.bin`
   - `tfm_s_ns_signed.bin`
   - `ddr_phy_signed.bin`
   - `bl2.stm32`

   **Note**: `TestApp_M33TD_CM33_NonSecure.bin` is the raw Non-Secure application output. `tfm_s_ns_signed.bin` is the signed combined TF-M Secure + Non-Secure image produced by the postbuild flow. For other boot device modes, the TF-M secure build commands and final deployed binary names must be adjusted as described later in the README.

---

## TestApp Task Configuration

The **TestApp Task** is responsible for running a sequence of M33 example tests. Each test follows these steps:
1. Acquire the required resources to run the test.
2. Initialize the necessary peripherals.
3. Execute the test.
4. Deinitialize the test and release the acquired resources.

### Test Categories
Tests are categorized into two types based on their execution requirements:
- **AUTO**: Tests that can be executed without external dependencies.
- **MANUAL**: Tests that require external dependencies or user interaction.

### Controlling Test Execution
While building the `TestApp_M33TD`, the test execution behavior can be controlled by passing the following CMake options or CubeIDE preprocessor definitions:

#### **Scenario 1: ENABLE_AUTO_TEST is ON (Set to 1)**
- **Behavior**:
  - The TestApp will execute automatically.
  - **AUTO** tests will be fully executed, including all steps (AcquireResource -> Init -> Run -> Deinit -> ReleaseResource).
  - **MANUAL** tests will be partially executed, performing only the resource acquisition and release steps (AcquireResource -> ReleaseResource).

#### **Scenario 2: ENABLE_AUTO_TEST is OFF (Set to 0)**
- **Behavior**:
  - The TestApp will execute tests fully based on **User Button 2** actions.
  - Both **AUTO** and **MANUAL** tests will be executed completely (AcquireResource -> Init -> Run -> Deinit -> ReleaseResource).

### How to Run Manual Tests

#### I3C Test
- Ensure the target I2C board is ready before executing the I3C controller test.
- Refer to the examples provided at `Firmware/Projects/STM32MP215F-DK/Examples/I3C` to set up the target I2C board.

#### SPI Test
- Ensure the slave SPI board is ready before executing the SPI Master test.
- Refer to the examples provided at `Firmware/Projects/STM32MP215F-DK/Examples/SPI` to set up the slave board for testing.

---

### Example CMake Options
```bash
-DENABLE_AUTO_TEST=ON
-DENABLE_AUTO_TEST=OFF
```

### Example CubeIDE Preprocessor Definitions
```c
ENABLE_AUTO_TEST=1
ENABLE_AUTO_TEST=0
```

---

#### UART Console Output

Below is a sample UART log output from the CM33 during the boot and runtime sequence:

```
[INF] Loading gpt header

[INF] welcome to MCUboot: ...
[INF] cpu: STM32MP215...
[INF] board: stm32mp215f dk
[INF] dts: stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts
[INF] boot device: sdmmc1
...
[INF] welcome to TF-M: ...
[INF] dts: stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts
...
[NS] [INF] Non-Secure system starting...
[NS] [INF] STM32Cube FW version: ...
[NS] [INF] [WdgMonitor] watchdog timeout: ...
[NS] [INF] [OpenAMP] waiting remote processor initialisation...
[NS] [INF] [RemoteProc] starting copro cpu@0... (pending)
[NS] [INF] [TestApp] Acquiring resources for test: TIMERS
[NS] [INF] [TestApp] Running test: TIMERS
[NS] [INF] [RemoteProc] copro cpu@0 started.
[NS] [INF] [TestApp] Test TIMERS: PASSED
[NS] [INF] [TestApp] Releasing resources for test: TIMERS
[NS] [INF] [TestApp] Acquiring resources for test: SPI
[NS] [INF] [TestApp] Skipping manual test in auto Test mode: SPI
[NS] [INF] [TestApp] Releasing resources for test: SPI
[NS] [INF] [TestApp] Acquiring resources for test: I3C
[NS] [INF] [TestApp] Skipping manual test in auto Test mode: I3C
[NS] [INF] [TestApp] Releasing resources for test: I3C
[NS] [INF] [TestApp] All tests completed.
[NS] [INF] [OpenAMP]  Ready for RPMsg communication
```

### Understanding TestApp Logs

This log demonstrates the CM33 boot sequence (MCUboot/TF-M), secure-non-secure handover, and the TestApp test execution flow.

> **Note:** The presence of `Skipping manual test in auto Test mode: ...` indicates the project was built with `ENABLE_AUTO_TEST=1` (for example via `-DENABLE_AUTO_TEST=ON`).

The log also reflects the specific firmware and configuration used for the device boot:

- **STM32Cube FW version**: `STM32Cube FW version: ...`
- **MCUboot version**: `welcome to MCUboot: ...`
- **TF-M version**: `welcome to TF-M: ...`
- **External device tree (DTS)**: `dts: ...-bl2.dts` and `dts: ...-s.dts` (compare with the `-DDTS_BOARD_...` options passed to the TF-M secure build)

Key runtime milestones:

- Display enabled (when `BUILD_CONFIG=FULL`): `[DisplayTask] Display is initialized and ready...`
- Watchdog monitor: `[WdgMonitor] watchdog timeout: ...`
- A35 bring-up: `[RemoteProc] starting ...` then `[RemoteProc] copro cpu@0 started.`
- TestApp progress:
  - Test start/finish lines (for example: `Running test: ...`, `Test ...: PASSED`, `All tests completed.`)
  - Auto-test behavior for MANUAL tests (for example: `Skipping manual test in auto Test mode: ...`)
- RPMsg ready: `[OpenAMP]  Ready for RPMsg communication`

To verify that the correct configuration is reflected, compare the DTS filename(s) and firmware versions shown in the log with the TF-M secure-build CMake options and external DTS files specified during the secure build.

### How to Test and Verify System Behavior

Once the system has completed boot (for example, you see `[RemoteProc] copro cpu@0 started.` and `[OpenAMP]  Ready for RPMsg communication`), you can run the checks below. For each test, look for the corresponding activity on the **CM33 UART console**.

1. **Verify TestApp automatic mode (`ENABLE_AUTO_TEST=1`)**
   - Build with `-DENABLE_AUTO_TEST=ON`.
   - Expected CM33 logs:
      - `[TestApp] ... Running test: ...` for **AUTO** tests
      - `[TestApp] Skipping manual test in auto Test mode: ...` for **MANUAL** tests
      - `[TestApp] All tests completed.`

2. **Verify TestApp manual mode (`ENABLE_AUTO_TEST=0`, USER2-triggered)**
   - Build with `-DENABLE_AUTO_TEST=OFF`.
   - Press **USER2** to trigger the TestApp sequence as described in [TestApp Task Configuration](#testapp-task-configuration).
   - Expected CM33 logs: both **AUTO** and **MANUAL** tests execute fully (no "Skipping manual test" messages).

3. **Button-triggered A35 reboot (USER2, very long press)**
   - Press **USER2** for **at least 5 seconds**.
   - When the OpenAMP power endpoint is enabled in the common stack, the Button Monitor task can trigger an M33-initiated reboot request to the remote processor.
   - Expected CM33 logs (may vary by build options):
      - `[OpenAMP] request remote processor reboot`
      - RemoteProc messages reflecting stop/start or recovery.

4. **Simulate a Linux crash and observe recovery**
   - On the Linux console (as `root`), run:
      ```bash
      sync; sleep 2; sync; echo c > /proc/sysrq-trigger
      ```
   - On the CM33 UART, RemoteProc should report crash detection and attempt recovery (stop/start sequence).

5. **Cold reset from Linux**
   - On Linux, run:
      ```bash
      reboot
      ```
   - You should see MCUboot/TF-M banners again on the CM33 UART.

6. **Warm reset from Linux**
   - On Linux, run:
      ```bash
      echo warm >> /sys/kernel/reboot/mode
      reboot
      ```
   - A warm reset typically restarts the A35 while keeping CM33 running; on the CM33 UART you should see the relevant SCMI notification and RemoteProc/OpenAMP re-init sequence (messages may vary by build options).

7. **Prepare the LowPowerMgr RPMsg endpoint on Linux**
   - TestApp enables the firmware low-power endpoint by default through OpenAMP. In the firmware, `OpenampTask_LowPowerEndpointRegister()` registers the RPMsg service named `low_power` at address `0x5A`.
   - On Linux, first bind or create an `rpmsg_char` endpoint for that `low_power` service. Once the endpoint is created, it appears as a `/dev/rpmsgX` node. The example commands below assume that node is `/dev/rpmsg2`.

8. **Suspend policy test: STOP2**
   - Limit the firmware suspend policy to STOP2 before Linux enters suspend:
      ```bash
      echo "LIMIT_PM_STOP2" > /dev/rpmsg2
      echo deep > /sys/power/mem_sleep
      rtcwake -m mem -s 10
      ```
   - Expected behavior: Linux requests suspend, the firmware keeps the low-power target at STOP2, and the system wakes up after the RTC timeout.

9. **Suspend policy test: LP_STOP2**
   - Allow the low-power manager to enter LP_STOP2 instead of plain STOP2:
      ```bash
      echo "LIMIT_PM_LP_STOP2" > /dev/rpmsg2
      echo deep > /sys/power/mem_sleep
      rtcwake -m mem -s 10
      ```
   - Expected behavior: Linux suspend still uses the RTC wake-up, but the firmware low-power target is constrained to LP_STOP2.

10. **Suspend policy test: LPLV_STOP2**
    - Allow the deepest STOP-class mode currently exposed over the low-power endpoint:
      ```bash
      echo "LIMIT_PM_LPLV_STOP2" > /dev/rpmsg2
      echo deep > /sys/power/mem_sleep
      rtcwake -m mem -s 10
      ```
    - Expected behavior: the firmware low-power manager accepts the deeper STOP-class mode and the system resumes after the RTC wake-up.

11. **RUN2 test with RTC wake-up**
    - `LIMIT_PM_DISABLED` disables low-power entry in the firmware. Linux can still enter its suspend path, but the M33 side stays in RUN2 while waiting for the wake-up source:
      ```bash
      echo "LIMIT_PM_DISABLED" > /dev/rpmsg2
      echo deep > /sys/power/mem_sleep
      rtcwake -m mem -s 10
      ```
    - Expected behavior: the CM33 logs should show the low-power request was rejected locally while the system still exercises the RUN2/D1 standby synchronization path.

12. **RUN2 test without RTC wake-up**
    - This variant keeps low-power entry disabled but lets Linux use its standard suspend flow without programming `rtcwake`:
      ```bash
      echo "LIMIT_PM_DISABLED" > /dev/rpmsg2
      echo deep > /sys/power/mem_sleep
      systemctl suspend
      ```
    - Expected behavior: wake-up depends on the platform wake-up source configured by Linux or the board environment. Use this flow to validate the no-RTC wake-up case.

## Error Behaviors
If an error occurs during initialization or system configuration at runtime, **LED3 will blink at a 100 ms interval** to indicate the error state.

## Assumptions


## Known Limitations


## Keywords
Security, TFM, Secure, SD Card, Non-Secure

---

## How to Flash Binaries

1. **Build/Compile the Project**
  Follow the [Build Procedure](#build-procedure) to generate the required binaries.

2. **Copy and Rename Generated Binaries**
   Before copying, **rename the generated binaries** (`bl2*.stm32`, `ddr_phy*_Signed.bin`, and `tfm-testapp-*_s_ns_Signed.bin`) according to the names specified in the **Required Binaries** section of the relevant flash layout under [Reference Use Cases for MP21-DK Board (TestApp Project)](#reference-use-cases-for-mp21-dk-board-testapp-project).

   Then, copy the renamed binaries to the following paths:
   ```
   <OSTL-IMAGE-PATH>/images/stm32mp2-m33td/arm-trusted-firmware-m/bl2
   ```
   - Place the renamed `bl2` binary in this directory.
   ```
   <OSTL-IMAGE-PATH>/images/stm32mp2-m33td/m33-firmware
   ```
   - Place the renamed `ddr_phy` and `tfm-testapp` binaries in this directory.

3. **Modify Flashlayout (TSV)**
   Select the relevant TSV file under `<OSTL-IMAGE-PATH>/images/stm32mp2-m33td/flashlayout_st-image-weston/optee` (see [Flash Layouts for MP21-DK Board](#flash-layouts-for-mp21-dk-board)).
   Replace the existing TF-M NS signed binary name with your renamed `tfm-testapp-..._s_ns_Signed.bin`.

---

## Different Boot Device Modes for MP21-DK Board

1. **sdcard_sdcard**:
   - MCUBOOT/TFM(S/NS) -> SD card
   - OSTL -> SD card

---

## Flash Layouts for MP21-DK Board

- `FlashLayout_sdcard_stm32mp215f-dk-cm33tdcid-ostl-optee.tsv`

**Note**: This TSV requires three binaries (BL2, DDR PHY firmware, and the combined TF-M S+NS signed image). Refer to the [Reference Use Cases for MP21-DK Board (TestApp Project)](#reference-use-cases-for-mp21-dk-board-testapp-project) section.

---

## Reference Use Cases for MP21-DK Board (TestApp Project)

### 1. FlashLayout_sdcard_stm32mp215f-dk-cm33tdcid-ostl-optee.tsv (sdcard_sdcard Boot Mode)

**Required Binaries**:
- `bl2-stm32mp215f-dk-cm33tdcid-ostl-sdcard.stm32`
- `ddr_phy-stm32mp215f-dk-cm33tdcid-ostl-sdcard_Signed.bin`
- `tfm-testapp-stm32mp215f-dk-cm33tdcid-ostl-sdcard-sdcard_s_ns_Signed.bin`

**Required CMake Options**:
```bash
-DSTM32_BOOT_DEV=sdmmc1
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-ns.dts

```
---

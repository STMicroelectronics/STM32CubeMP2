# VisualWatchdog_M33TD Demonstration Application

---

## Application Description

The VisualWatchdog_M33TD demonstration highlights the STM32MP2 M33TDCID profile with Cortex-M33 as the primary CPU and Cortex-A35 as a managed coprocessor. It is a utility-integrated project built on `Utilities/M33TD_NSAppCore`, with board-specific display and peripheral drivers kept in the project under `CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/`.

This demo uses a fixed visual configuration: display, overlay rendering, splash animation assets, OpenAMP transport, button handling, and the watchdog-oriented UI are part of the default setup.

---

## Key Features

- **FreeRTOS Multitasking**: Runs the NS application as a multi-task FreeRTOS system.
- **Utility Stack Integration**: Uses `Utilities/M33TD_NSAppCore` for shared bootstrap and task logic.
- **Visual Display Pipeline**: Keeps the display task enabled with overlay content and splash animation assets in the project profile.
- **OpenAMP and Button Handling**: Enables the RPMsg transport together with board-side button interaction.
- **A35 Coprocessor Management**: Uses the RemoteProc task to manage the A35 lifecycle through TF-M secure services.
- **Watchdog Visualization**: Focuses on watchdog-oriented visual status rendering on the local display.

---

## Purpose

This is a FreeRTOS-based multi-task application running in the NS environment while TF-M runs in the secure environment and provides secure services.

In the current STM32MP215F-DK VisualWatchdog profile, `NSCoreApp_Init()` starts the enabled utility tasks below:

1. **NSCoreApp (Bootstrap)**:
   - Initializes the common stack and starts the enabled tasks.

2. **Logger Task**:
   - Centralized logging, with optional real-time output.

3. **UserApp Task**:
   - Keeps the example local application activity enabled.

4. **Button Monitor Task**:
   - Monitors the USER button and dispatches project-side actions.

5. **Display Task**:
   - Drives the display pipeline and watchdog-oriented UI updates.

6. **SCMI Manager Task**:
   - Handles SCMI notifications and related TF-M forwarding.

7. **OpenAMP Task**:
   - Provides the RPMsg transport used by the VisualWatchdog profile.

8. **RemoteProc Task**:
   - Manages A35 lifecycle control and recovery flows.

9. **Watchdog Monitor Task**:
   - Supervises the watchdog path and feeds the visual status model.

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

### Non-Secure M33 Firmware Build (VisualWatchdog_M33TD Application)
1. Navigate to `Firmware/Projects/STM32MP215F-DK/Demonstrations/VisualWatchdog_M33TD`.
2. Run the following command:
   ```bash
   cmake -G"Unix Makefiles" -B build -DTFM_BUILD_DIR=<TFM_BUILD_DIRECTORY>
   ```
   - If `TFM_BUILD_DIR` is not specified, the default path `Firmware/Middlewares/Third_Party/trusted-firmware-m/config_default` is used.
   - The display panel, overlay rendering, and splash animation assets are enabled by the project configuration.
3. Build the project:
   ```bash
   make -C build all
   ```

---

## To Build Native STM32CubeIDE Project

### Project Structure
```
VisualWatchdog_M33TD
├── VisualWatchdog_M33TD_CM33
│   ├── VisualWatchdog_M33TD_CM33_NonSecure
│   └── VisualWatchdog_M33TD_CM33_trusted-firmware-m
```

**Note**: Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/How_to_create_an_M33-TD_boot_project_using_STM32CubeIDE#) for project import guidance.

### Build Procedure

#### Secure TFM Firmware Build
1. Configure the TF-M CMake project with the same platform and DTS values used by the command-line flow.
2. Run `CMake Configure` on `VisualWatchdog_M33TD_CM33_trusted-firmware-m`.
3. Build the TF-M CMake project.

#### Non-Secure STM32CubeIDE Project Build
1. Select the matching NonSecure build configuration.
2. Build `VisualWatchdog_M33TD_CM33_NonSecure`.

---

## Output

- The generated binaries will be located in the `bin` folder:
```
   cd bin/
```
   - `VisualWatchdog_M33TD_CM33_NonSecure.bin`
   - `tfm_s_ns_signed.bin`
   - `ddr_phy_signed.bin`
   - `bl2.stm32`

   **Note**: `VisualWatchdog_M33TD_CM33_NonSecure.bin` is the raw Non-Secure application output. `tfm_s_ns_signed.bin` is the signed combined TF-M Secure + Non-Secure image produced by the postbuild flow. For other boot device modes, the TF-M secure build commands and final deployed binary names must be adjusted as described later in the README.

---

For detailed instructions on flashing the generated binaries to your device, see [How to Flash Binaries](#how-to-flash-binaries).

---

#### UART Console Output

Below is a sample UART log output from the CM33 during the boot and runtime sequence:

```
[WRN] This device was provisioned with dummy keys.

[WRN] This device is NOT SECURE

[INF] NV_MM_COUNTER_INIT: counters are initialized.

[INF] PSA Crypto init done, sig_type: EC-P256
[INF] Primary   slot: version=0.1.0+0
[INF] Image 1 Secondary slot: Image not found
[INF] Image 1 RAM loading to 0xe060000 is succeeded.
[INF] Image 1 loaded from the primary slot
[INF] BL2: image 1, enable DDR-FW
[INF] Primary   slot: version=2.1.0+0
[INF] Image 0 Secondary slot: Image not found
[INF] Image 0 RAM loading to 0x80000000 is succeeded.
[INF] Image 0 loaded from the primary slot
[INF] Bootloader chainload address offset: 0x104400
[INF] Jumping to the first image slot
[INF] init:pmic@33 STPMIC:20 V1.1
[INF] Enable Macronix quad support
[INF] welcome to TF-M: v2.1.3-stm32mp-r2-rc7
[INF] board: stm32mp215f dk
[INF] dts: stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts
Booting TF-M v2.1.3
[WAR] This device was provisioned with dummy keys.
[WAR] This device is NOT SECURE
[Sec Thread] Secure image initializing!
TF-M isolation level is: 0x00000002
[    0.000000] SCP-firmware v2.13.0-stm32mp-r3-rc4
[    0.000000]
[    0.000000] [FWK] Module initialization complete!
Creating an empty ITS flash layout.
[INF][PS] Encryption alg: 0x5500100
[INF][Crypto] Init HW accelerator...
[INF][Crypto] Init HW accelerator... complete.
[NS] [INF] Non-Secure system starting...
[NS] [INF] STM32Cube FW version: v1.2.0-rc0
[NS] [INF] [DisplayTask] Display is initialized and ready...
[NS] [INF] [WdgMonitor] watchdog timeout: 120000ms
[NS] [INF] [OpenAMP] waiting remote processor initialisation...
[NS] [INF] [RemoteProc] starting copro cpu@0... (pending)
[NS] [INF] [RemoteProc] copro cpu@0 started.
[NS] [INF] [OpenAMP]  Ready for RPMsg communication
```

### Understanding VisualWatchdog Logs

This log demonstrates the CM33 boot sequence (MCUboot/TF-M), provisioning status, secure-non-secure handover, and the main VisualWatchdog runtime tasks (DisplayTask, Watchdog Monitor, RemoteProc, OpenAMP).

> **Note:** The `[RemoteProc] starting copro cpu@0... (pending)` message followed by `[RemoteProc] copro cpu@0 started.` indicates the A35 coprocessor was started during boot. This behavior is typically enabled by the `REMOTE_PROC_AUTO_START=ON` CMake option, as described in the [Non-Secure M33 Firmware Build (VisualWatchdog_M33TD Application)](#non-secure-m33-firmware-build-visualwatchdog_m33td-application) section.

The log also reflects the specific firmware and configuration used for the device boot:

- **STM32Cube FW version**: `STM32Cube FW version: ...`
- **TF-M version**: `welcome to TF-M: ...`
- **External device tree (DTS)**: `dts: ...-sdcard-s.dts` (compare with the `-DDTS_BOARD_S=...` DTS passed to the TF-M secure build)
- **Key runtime milestones**:
   - Display enabled: `[DisplayTask] Display is initialized and ready...`
   - A35 bring-up: `[RemoteProc] ... started.`
   - RPMsg ready: `[OpenAMP]  Ready for RPMsg communication`
   - Watchdog monitor: `[WdgMonitor] watchdog timeout: ...`

To verify that the correct configuration is reflected, compare the DTS filename(s) and firmware versions shown in the log with the TF-M secure-build CMake options and external DTS files specified during the secure build (see [How to Generate Binaries for Different Boot Modes](#how-to-generate-binaries-for-different-boot-modes)).

VisualWatchdog also implements remote processor crash detection and automatic recovery; see [How to Test and Verify System Behavior](#how-to-test-and-verify-system-behavior).

### How to Test and Verify System Behavior

Once the system has completed boot (e.g., you see `[RemoteProc] copro cpu@0 started.` and, when enabled, `[OpenAMP]  Ready for RPMsg communication`), you can run the checks below. For each test, look for the corresponding activity on the **CM33 UART console**.

1. **Linux HMI-triggered A35 reboot and status-bar transition**
    - On the Linux HMI, press the UI Launcher `A35 Reboot/Crash` reboot control.
    - Expected local display behavior during the restart sequence:
       - the A35 status bar changes from **Running** to **Rebooting**
       - the reboot animation is shown while the A35 is restarting
    - Once the reboot completes and Linux is back up, the local status bar returns to **Running**.
    - On the CM33 UART, you may also see the A35 stop/start or recovery sequence, depending on timing and build options.

2. **Button-triggered A35 reboot (USER2, very long press)**
    - Press **USER2** for **at least 5 seconds**.
    - Expected behavior:
       - `[OpenAMP] request remote processor reboot`
       - the A35 restart sequence may be reflected by RemoteProc messages and the local status bar may briefly show **Rebooting** before returning to **Running**

3. **Simulate a Linux crash and observe recovery**
    - On the Linux console (as `root`), run:
       ```bash
       sync; sleep 2; sync; echo c > /proc/sysrq-trigger
       ```
    - This forces a kernel crash on the A35 side. On the CM33 UART, RemoteProc should report crash detection and attempt recovery (for example: `Crash detected ... Attempting recovery...`, then a stop/restart sequence).

4. **Cold reset from Linux**
    - On Linux, run:
       ```bash
       reboot
       ```
    - A cold reset typically reboots the full platform; you should see TF-M/MCUboot banners again on the CM33 UART. Depending on timing, you may also see an SCMI notification such as `SYS_POWER_COLD_RESET` before the reset.

5. **Warm reset from Linux**
    - On Linux, run:
       ```bash
       echo warm >> /sys/kernel/reboot/mode
       reboot
       ```
    - A warm reset typically restarts the A35 while keeping CM33 running; on the CM33 UART you should see `SYS_POWER_WARM_RESET` and a RemoteProc stop/start sequence. When OpenAMP is enabled, you should also see `OpenAMP reinit requested`.

---

## Error Behaviors
If an error occurs during initialization or system configuration at runtime, **LED3 will blink at a 100 ms interval** to indicate the error state.

---

## Assumptions

---


## Known Limitations

---


## Keywords
Security, TFM, Secure, SD Card, Non-Secure

---

## How to Flash Binaries

1. **Build/Compile the Project**  
  Follow the [Build Procedure](#build-procedure) to generate the required binaries.

2. **Copy and Rename Generated Binaries**  
   Before copying, **rename the generated binaries** (`bl2.stm32`, `ddr_phy_signed.bin`, and `tfm_s_ns_signed.bin`) according to the names specified in the **Required Binaries** section of the relevant flash layout under [Reference Use Cases for MP21-DK Board (VisualWatchdog Project)](#reference-use-cases-for-mp21-dk-board-visualwatchdog-project).  
   Then, navigate to the `bin/` folder and copy the renamed binaries to the following paths:
   ```
   <OSTL-IMAGE-PATH>/images/stm32mp2-m33td/arm-trusted-firmware-m/bl2
   ```
   - Place the renamed `bl2` binary in this directory.
   ```
   <OSTL-IMAGE-PATH>/images/stm32mp2-m33td/m33-firmware
   ```
   - Place the renamed `ddr_phy` and `tfm-visualwatchdog` binaries in this directory.

3. **Modify Flashlayout (TSV)**  
   Select the relevant TSV file under `<OSTL-IMAGE-PATH>/images/stm32mp2-m33td/flashlayout_st-image-weston/optee` as per specific dev mode (see [Flash Layouts for MP21-DK Board](#flash-layouts-for-mp21-dk-board)).
   And then modify the TSV file, by replacing the existing TF-M NS signed binary name with the name of your renamed `tfm-visualwatchdog-..._s_ns_Signed.bin` that corresponds to your chosen boot mode.

4. **Connect the Device**  
  Use a Type-C cable to connect the device to the Type-C connector.

5. **Flash the Image**  
  Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/) for detailed flashing instructions.

6. **Perform a Power-On Reset**  
  After flashing, perform a power-on reset to complete the process.

---

## Different Boot Device Modes for MP21-DK Board

The MP21-DK board supports the following boot device modes:

1. **sdcard_sdcard**:  
   - MCUBOOT/TFM(S/NS) -> SD card  
   - OSTL -> SD card  

---

## Flash Layouts for MP21-DK Board

For the MP21-DK board, only one TSV flavor is provided to flash under the M33TDCID profile:

- `FlashLayout_sdcard_stm32mp215f-dk-cm33tdcid-ostl-optee.tsv`

**Note**: Each TSV requires three sets of binaries. Refer to the [Reference Use Cases for MP21-DK Board (VisualWatchdog Project)](#reference-use-cases-for-mp21-dk-board-visualwatchdog-project) section for details.

---

## How to Generate Binaries for Different Boot Modes

1. **Configure and Build TFM Secure**:  
  Use specific CMake options defined for each boot mode as described in the **Required CMake Options** section under [Reference Use Cases for MP21-DK Board (VisualWatchdog Project)](#reference-use-cases-for-mp21-dk-board-visualwatchdog-project).  

2. **Build VisualWatchdog Project**:  
   Compile the VisualWatchdog project after building the TFM secure binaries.  

3. **Rename Generated Binaries**:  
   Rename the binaries in the `bin/` folder to match the required binaries for the specific TSV as described in the **Required Binaries** section under [Reference Use Cases for MP21-DK Board (VisualWatchdog Project)](#reference-use-cases-for-mp21-dk-board-visualwatchdog-project).  

---

## Reference Use Cases for MP21-DK Board (VisualWatchdog Project)

### 1. FlashLayout_sdcard_stm32mp215f-dk-cm33tdcid-ostl-optee.tsv (sdcard_sdcard Boot Mode)

**Required Binaries**:  
- `bl2-stm32mp215f-dk-cm33tdcid-ostl-sdcard.stm32`  
- `ddr_phy-stm32mp215f-dk-cm33tdcid-ostl-sdcard_Signed.bin`  
- `tfm-visualwatchdog-stm32mp215f-dk-cm33tdcid-ostl-sdcard-sdcard_s_ns_Signed.bin`  

**Required CMake Options**:  
```bash
-DSTM32_BOOT_DEV=sdmmc1  # SD card
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-ns.dts
```
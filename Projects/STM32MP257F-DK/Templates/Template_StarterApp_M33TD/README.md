# Template_StarterApp_M33TD Template Project

---

## Application Description

`Template_StarterApp_M33TD` is the STM32MP257F-DK starter template for a Cortex-M33 Non-Secure application built on `Utilities/M33TD_NSAppCore`.

It provides a utility-integrated baseline: the required NSAppCore core tasks stay enabled, a simple UserApp task is added by default, and project-specific drivers remain local to the template under `CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/`.

The intent is to give a clean starting point for a new M33TD project with the minimal common bootstrap, TF-M integration, watchdog supervision, and A35 lifecycle handling needed to bring up the full M33TD-OSTL ecosystem.

---

## Default Task Set

`NSCoreApp_Init()` starts the required utility core tasks plus the template's example UserApp task:

---

1. **NSCoreApp (Bootstrap)**:
   - Initializes the common stack and starts the enabled tasks.

2. **Logger Task**:
   - Centralized logging, with optional real-time debug output.

3. **SCMI Manager Task**:
   - Handles SCMI notifications and related TF-M forwarding.

4. **RemoteProc Task**:
   - Manages the A35 coprocessor lifecycle through TF-M secure services.
   - Auto-start at boot is controlled by `REMOTE_PROC_AUTO_START`.

5. **Watchdog Monitor Task**:
   - Supervises watchdog-related health handling.

6. **UserApp Task**:
   - Provides the example project-side application hook enabled by default in the template.

Additional task enables are configured in the project headers and can be adjusted as the new application grows.

6. **Watchdog Monitor Task**:
   - Supervises system health and watchdog-related handling.

---


## Prerequisite Hardware & Software Environment Setup

- **Trusted Firmware-M**: The source code must be installed under the `Middlewares/Third_Party` directory with the path `Middlewares/Third_Party/trusted-firmware-m`. Ensure the correct version is used as described in the STM32 MPU release note.
- **Supported Devices**: This example runs on STM32MP25xx devices and has been tested with the STMicroelectronics STM32MP257F-DK board. It can be tailored to other supported devices and development boards.
- **ST-Link Connection**: Connect the ST-Link cable to the PC USB port to display traces.

### Software Versions
- **Trusted Firmware-M (TFM)**: Refer to the [Trusted Firmware-M wiki](https://wiki.st.com/stm32mpu/wiki/Category:Trusted_Firmware-M) for recommended versions and integration details.
- **External Device Tree (externalDT)**: See the [External Device Tree wiki](https://wiki.st.com/stm32mpu/wiki/External_device_tree) for guidance on obtaining and using external DT sources.
- **STM32CubeIDE**: For supported IDE versions and ecosystem information, refer the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/).

---

## Artifact flow (build → sign → deploy)

This is a high-level view; the later sections in this README give the board-specific filenames and flashing steps.

<pre>
┌──────────────┐
│ TF-M build   │
└─┬────────────┘
  +-> bl2*.stm32
  +-> ddr_phy*_Signed.bin
  +-> tfm_s.bin

┌───────────────────┐
│ CM33-NS app build │
└─┬─────────────────┘
  +-> ${PROJECT_NAME}.bin

┌─────────────────────────────┐
│ postbuild (assemble + sign) │
└─┬───────────────────────────┘
  | inputs: ${PROJECT_NAME}.bin + tfm_s.bin
  +-> tfm-*_s_ns_Signed.bin

Deploy into OSTL image tree
   bl2*.stm32            -> .../images/stm32mp2-m33td/arm-trusted-firmware-m/bl2
   ddr_phy*_Signed.bin   -> .../images/stm32mp2-m33td/m33-firmware
   tfm-*_s_ns_Signed.bin -> .../images/stm32mp2-m33td/m33-firmware
</pre>
  
## To Build CMake Project for M33TDCID (Through Command Line)

### Compilation Instructions
- Export the path of any ARM cross-compiler toolchain with `arm-none-eabi-gcc` to your PC's PATH environment variable.
  - Example for STM32CubeIDE's 1.19.0.25A9 Toolchain:
    ```
    C:\ST\STM32CubeIDE_1.19.0.25A9\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.12.3.rel1.win32_1.1.0.202501171655\tools\bin
    ```

---

## Build Procedure

### Secure Build (TFM)
1. Navigate to `Firmware/Middlewares/Third_Party/trusted-firmware-m`.
2. For SD card development mode, execute the following command:
   ```bash
   cmake -B config_default -G"Unix Makefiles" -DTFM_PLATFORM=stm/stm32mp257f_dk -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake -DSTM32_BOOT_DEV=sdmmc1 -DTFM_PROFILE=profile_medium -DSTM32_M33TDCID=ON -DCMAKE_BUILD_TYPE=Relwithdebinfo -DNS=OFF -DDTS_EXT_DIR=<EXT_DT_DIR> -DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-dk-cm33tdcid-ostl-sdcard-bl2.dts -DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-dk-cm33tdcid-ostl-sdcard-s.dts 
   -DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-dk-cm33tdcid-ostl-ns.dts 
   ```
3. Build the project:
   ```bash
   cmake --build config_default -- install
   ```

**Note**: The external DT repository is placed in the `Utilities` directory.

---

### Non-Secure M33 Firmware Build (Template_StarterApp_M33TD Application)
1. Navigate to `Firmware/Projects/STM32MP257F-DK/Templates/Template_StarterApp_M33TD`.
2. Run the following command:
   ```bash
      cmake -G"Unix Makefiles" -B build -DTFM_BUILD_DIR=<TFM_BUILD_DIRECTORY> -DREMOTE_PROC_AUTO_START=ON
   ```
      - If `TFM_BUILD_DIR` is not specified, the default path `Firmware/Middlewares/Third_Party/trusted-firmware-m/config_default` will be used, assuming the TFM Secure Build step is completed.
      - By default:
        - `REMOTE_PROC_AUTO_START` is set to `1` for Template_StarterApp_M33TD unless explicitly provided

3. Build the project:
   ```bash
   make -C build all
   ```

---

## To Build Native STM32CubeIDE Project

### Project Structure
```
Template_StarterApp_M33TD
├── Template_StarterApp_M33TD_CM33
│   ├── Template_StarterApp_M33TD_CM33_NonSecure (Non-Secure STM32CubeIDE M33 project)
│   └── Template_StarterApp_M33TD_CM33_trusted-firmware-m (Secure CMake project)
```

**Note**: Refer to [this wiki](https://wiki.st.com/stm32mpu/wiki/How_to_create_an_M33-TD_boot_project_using_STM32CubeIDE#) for instructions on importing a CMake project.

### Build Procedure

#### Secure TFM Firmware Build
1. Configure the CMake build options:
    - Navigate to `Template_StarterApp_M33TD_CM33_trusted-firmware-m` and update the CMake settings:
      ```
      -DDEBUG_AUTHENTICATION=FULL  # Enabled for Debug Purpose, Default: this option is removed
      -DTFM_PLATFORM=stm/stm32mp257f-dk
      -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake
      -DSTM32_BOOT_DEV=sdmmc1  # Building TFM for "sdcard_sdcard" bootdevice mode
      -DTFM_PROFILE=profile_medium
      -DSTM32_M33TDCID=ON
      -DCMAKE_BUILD_TYPE=Relwithdebinfo
      -DNS=OFF
      -DDTS_EXT_DIR=../../../../../../../../../Firmware/Utilities/dt-stm32mp
      -DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-dk-cm33tdcid-ostl-sdcard-bl2.dts  # External DT file for sdcard_sdcard bootdevice mode 
      -DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-dk-cm33tdcid-ostl-sdcard-s.dts          # External DT file for sdcard_sdcard bootdevice mode 
      -DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-dk-cm33tdcid-ostl-ns.dts               # External DT file common for all bootdevice modes
      ```
2. Configure the TFM CMake project:
    - Right-click on `Template_StarterApp_M33TD_CM33_trusted-firmware-m` -> `CMake Configure`.
3. Build the TFM CMake project:
    - Right-click on `Template_StarterApp_M33TD_CM33_trusted-firmware-m` -> `Build Project`.

#### Non-Secure STM32CubeIDE Project Build
1. Build the project:
    - Select the build configuration as per the TFM version:
      - Choose `CM33TDCID_m33_ns_tfm_s_sign`.
    - Right-click on `Template_StarterApp_M33TD_CM33_NonSecure` -> `Build Project`.

---

## Output

- The generated binaries will be located in the `bin` folder:
```
   cd bin/
   ```
   - `Template_StarterApp_M33TD_CM33_NonSecure.bin`
   - `tfm_s_ns_signed.bin`
   - `ddr_phy_signed.bin`
   - `bl2.stm32`

   **Note**: For other boot device modes, TFM Secure Build commands need to be adjusted, and the generated binary should be renamed accordingly. Refer to the [How to Generate Binaries for Different Boot Modes](#how-to-generate-binaries-for-different-boot-modes) section.

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
[INF] board: stm32mp257f dk
[INF] dts: stm32mp257f-dk-cm33tdcid-ostl-sdcard-s.dts
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
[NS] [INF] [WdgMonitor] watchdog timeout: 120000ms
[NS] [INF] [RemoteProc] starting copro cpu@0... (pending)
[NS] [INF] [RemoteProc] copro cpu@0 started.
```

### Understanding Template StarterApp Logs

This log demonstrates the CM33 boot sequence (MCUboot/TF-M), provisioning status, secure→non-secure handover, and the core Template StarterApp runtime tasks (Watchdog Monitor and RemoteProc).

> **Note:** The `[RemoteProc] starting copro cpu@0... (pending)` message followed by `[RemoteProc] copro cpu@0 started.` indicates the A35 coprocessor was started during boot. This behavior is typically enabled by the `REMOTE_PROC_AUTO_START=ON` CMake option, as described in the [Non-Secure M33 Firmware Build (Template_StarterApp_M33TD Application)](#non-secure-m33-firmware-build-template_starterapp_m33td-application) section.

The log also reflects the specific firmware and configuration used for the device boot:

- **STM32Cube FW version**: `STM32Cube FW version: ...`
- **TF-M version**: `welcome to TF-M: ...`
- **External device tree (DTS)**: `dts: ...-sdcard-s.dts` (compare with the `-DDTS_BOARD_S=...` DTS passed to the TF-M secure build)
- **Key runtime milestones**:
   - A35 bring-up: `[RemoteProc] ... started.`
   - Watchdog monitor: `[WdgMonitor] watchdog timeout: ...`

To verify that the correct configuration is reflected, compare the DTS filename(s) and firmware versions shown in the log with the TF-M secure-build CMake options and external DTS files specified during the secure build (see [How to Generate Binaries for Different Boot Modes](#how-to-generate-binaries-for-different-boot-modes)).

### How to Test and Verify System Behavior

Once the system has completed boot (e.g., you see `[RemoteProc] copro cpu@0 started.`), you can run the checks below. For each test, look for the corresponding activity on the **CM33 UART console**.

1. **Simulate a Linux crash and observe recovery**
    - On the Linux console (as `root`), run:
       ```bash
       sync; sleep 2; sync; echo c > /proc/sysrq-trigger
       ```
    - This forces a kernel crash on the A35 side. On the CM33 UART, RemoteProc should report crash detection and attempt recovery (for example: `Crash detected ... Attempting recovery...`, then a stop/restart sequence).

2. **Cold reset from Linux**
    - On Linux, run:
       ```bash
       reboot
       ```
    - A cold reset typically reboots the full platform; you should see TF-M/MCUboot banners again on the CM33 UART. Depending on timing, you may also see an SCMI notification such as `SYS_POWER_COLD_RESET` before the reset.

3. **Warm reset from Linux**
    - On Linux, run:
       ```bash
       echo warm >> /sys/kernel/reboot/mode
       reboot
       ```
    - A warm reset typically restarts the A35 while keeping CM33 running; on the CM33 UART you should see `SYS_POWER_WARM_RESET` and a RemoteProc stop/start sequence.

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
   Before copying, **rename the generated binaries** (`bl2.stm32`, `ddr_phy_signed.bin`, and `tfm_s_ns_signed.bin`) according to the names specified in the **Required Binaries** section of the relevant flash layout under [Reference Use Cases for MP25-DK Board (Template_StarterApp Project)](#reference-use-cases-for-mp25-dk-board-starterapp-project).  
   Then, navigate to the `bin/` folder and copy the renamed binaries to the following paths:
   ```
   <OSTL-IMAGE-PATH>/images/stm32mp2-m33td/arm-trusted-firmware-m/bl2
   ```
   - Place the renamed `bl2` binary in this directory.
   ```
   <OSTL-IMAGE-PATH>/images/stm32mp2-m33td/m33-firmware
   ```
   - Place the renamed `ddr_phy` and `tfm-starterapp` binaries in this directory.

3. **Modify Flashlayout (TSV)**  
   Select the relevant TSV file under `<OSTL-IMAGE-PATH>/images/stm32mp2-m33td/flashlayout_st-image-weston/optee` as per specific dev mode (see [Flash Layouts for MP25-DK Board](#flash-layouts-for-mp25-dk-board)).
   And then modify the TSV file, by replacing the existing TF-M NS signed binary name with the name of your renamed `tfm-starterapp-..._s_ns_Signed.bin` that corresponds to your chosen boot mode.

4. **Connect the Device**  
  Use a Type-C cable to connect the device to the Type-C connector.

5. **Flash the Image**  
  Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/) for detailed flashing instructions.

6. **Perform a Power-On Reset**  
  After flashing, perform a power-on reset to complete the process.

---

## Different Boot Device Modes for MP25-DK Board

The MP25-DK board supports the following boot device modes:

1. **sdcard_sdcard**:  
   - MCUBOOT/TFM(S/NS) → SD card  
   - OSTL → SD card  

---

## Flash Layouts for MP25-DK Board

For the MP25-DK board, five TSV flavors are provided to flash under the M33TDCID profile:

- `FlashLayout_sdcard_stm32mp257f-dk-cm33tdcid-ostl-optee.tsv`

**Note**: Each TSV requires three sets of binaries. Refer to the [Reference Use Cases for MP25-DK Board (Template_StarterApp Project)](#reference-use-cases-for-mp25-dk-board-starterapp-project) section for details.

---

## How to Generate Binaries for Different Boot Modes

1. **Configure and Build TFM Secure**:  
  Use specific CMake options defined for each boot mode as described in the **Required CMake Options** section under [Reference Use Cases for MP25-DK Board (Template_StarterApp Project)](#reference-use-cases-for-mp25-dk-board-starterapp-project).  

2. **Build Template_StarterApp Project**:  
   Compile the Template_StarterApp project after building the TFM secure binaries.  

3. **Rename Generated Binaries**:  
   Rename the binaries in the `bin/` folder to match the required binaries for the specific TSV as described in the **Required Binaries** section under [Reference Use Cases for MP25-DK Board (Template_StarterApp Project)](#reference-use-cases-for-mp25-dk-board-starterapp-project).  

---

## Reference Use Cases for MP25-DK Board (Template_StarterApp Project)

### 1. FlashLayout_sdcard_stm32mp257f-dk-cm33tdcid-ostl-optee.tsv (sdcard_sdcard Boot Mode)

**Required Binaries**:  
- `bl2-stm32mp257f-dk-cm33tdcid-ostl-sdcard.stm32`  
- `ddr_phy-stm32mp257f-dk-cm33tdcid-ostl-sdcard_Signed.bin`  
- `tfm-starterapp-stm32mp257f-dk-cm33tdcid-ostl-sdcard-sdcard_s_ns_Signed.bin`  

**Required CMake Options**:  
```bash
-DSTM32_BOOT_DEV=sdmmc1  # SD card
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-dk-cm33tdcid-ostl-sdcard-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-dk-cm33tdcid-ostl-sdcard-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-dk-cm33tdcid-ostl-ns.dts
```

---

### 1. FlashLayout_emmc_stm32mp257f-dk-cm33tdcid-ostl-optee.tsv (emmc_emmc Boot Mode)

**Required Binaries**:  
- `bl2-stm32mp257f-dk-cm33tdcid-ostl-emmc.stm32`  
- `ddr_phy_signed-stm32mp257f-dk-cm33tdcid-ostl-emmc.bin`  
- `tfm-starterapp-stm32mp257f-dk-cm33tdcid-ostl-emmc-emmc_s_ns_signed.bin`  

**Required CMake Options**:  
```bash
-DSTM32_BOOT_DEV=sdmmc2  # eMMC
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-dk-cm33tdcid-ostl-emmc-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-dk-cm33tdcid-ostl-emmc-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-dk-cm33tdcid-ostl-ns.dts
```
# Template_StarterApp_M33TD Template Project

---

## Application Description

`Template_StarterApp_M33TD` is the STM32MP257F-EV1 starter template for a Cortex-M33 Non-Secure application built on `Utilities/M33TD_NSAppCore`.

It provides a utility-integrated baseline: the required NSAppCore core tasks stay enabled, a simple UserApp task is added by default, and project-specific drivers remain local to the template under `CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/`.

The intent is to give a clean starting point for a new M33TD project with the minimal common bootstrap, TF-M integration, watchdog supervision, and A35 lifecycle handling needed to bring up the full M33TD-OSTL ecosystem.

---

## Default Task Set

`NSCoreApp_Init()` starts the required utility core tasks plus the template's example UserApp task:

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

---

## Prerequisite Hardware & Software Environment Setup

- **Trusted Firmware-M**: Install the source code under `Middlewares/Third_Party/trusted-firmware-m`.
- **Supported Devices**: This template targets STM32MP25xx devices and is provided for STM32MP257F-EV1.
- **ST-Link Connection**: Connect the ST-Link cable to the PC USB port to display traces.

### Software Versions
- **Trusted Firmware-M (TFM)**: Refer to the [Trusted Firmware-M wiki](https://wiki.st.com/stm32mpu/wiki/Category:Trusted_Firmware-M).
- **External Device Tree (externalDT)**: See the [External Device Tree wiki](https://wiki.st.com/stm32mpu/wiki/External_device_tree).
- **STM32CubeIDE**: Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/).

---

## To Build CMake Project for M33TDCID (Through Command Line)

### Compilation Instructions
- Ensure an ARM cross-compiler toolchain with `arm-none-eabi-gcc` is available in `PATH`.
- When using `-G"Unix Makefiles"` on Windows, also ensure `make` is available in `PATH`.

---

## Build Procedure

### Secure Build (TFM)
1. Navigate to `Firmware/Middlewares/Third_Party/trusted-firmware-m`.
2. For SD card development mode, execute the TF-M configure command matching STM32MP257F-EV1 and your selected boot-device DTS files.
3. Build the project:
	```bash
	cmake --build config_default -- install
	```

**Note**: The external DT repository is placed in the `Utilities` directory.

---

### Non-Secure M33 Firmware Build (Template_StarterApp_M33TD Application)
1. Navigate to `Firmware/Projects/STM32MP257F-EV1/Templates/Template_StarterApp_M33TD`.
2. Run the following command:
	```bash
	cmake -G"Unix Makefiles" -B build -DTFM_BUILD_DIR=<TFM_BUILD_DIRECTORY>
	```
	- If `TFM_BUILD_DIR` is not specified, the default path `Firmware/Middlewares/Third_Party/trusted-firmware-m/config_default` is used.
	- Current project CMake options:
	  - `-DREMOTE_PROC_AUTO_START=ON|OFF` (default: `ON`)
	  - `-DREALTIME_DEBUG_LOG_ENABLED=ON|OFF` (default: `OFF`)
	  - `-DFAULT_EXCEPTION_ENABLE=ON|OFF` (default: `ON`)
	  - `-DFAULT_EXCEPTION_BACKTRACE_ENABLE=ON|OFF` (default: `ON`)
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
│   ├── Template_StarterApp_M33TD_CM33_NonSecure
│   └── Template_StarterApp_M33TD_CM33_trusted-firmware-m
```

**Note**: Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/How_to_create_an_M33-TD_boot_project_using_STM32CubeIDE#) for project import guidance.

### Build Procedure

#### Secure TFM Firmware Build
1. Configure the TF-M CMake project with the same platform and DTS values used by the command-line flow.
2. Run `CMake Configure` on `Template_StarterApp_M33TD_CM33_trusted-firmware-m`.
3. Build the TF-M CMake project.

#### Non-Secure STM32CubeIDE Project Build
1. Select the matching NonSecure build configuration.
2. Build `Template_StarterApp_M33TD_CM33_NonSecure`.

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

**Note**: `Template_StarterApp_M33TD_CM33_NonSecure.bin` is the raw Non-Secure application output. `tfm_s_ns_signed.bin` is the signed combined TF-M Secure + Non-Secure image produced by the postbuild flow.

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
[INF] board: stm32mp257f eval1
[INF] dts: stm32mp257f-ev1-cm33tdcid-ostl-sdcard-s.dts
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

### Understanding Template Logs

This log demonstrates the CM33 boot sequence (MCUboot/TF-M), provisioning status, secure-non-secure handover, and the baseline template runtime tasks (Watchdog Monitor and RemoteProc).

The log also reflects the specific firmware and configuration used for the device boot:

- **STM32Cube FW version**: `STM32Cube FW version: ...`
- **TF-M version**: `welcome to TF-M: ...`
- **External device tree (DTS)**: `dts: ...`
- **Key runtime milestones**:
  - Watchdog monitor startup: `[WdgMonitor] watchdog timeout: ...`
  - A35 bring-up: `[RemoteProc] ... started.`

Any additional UserApp trace output depends on how the template's project-side hook is implemented.

### How to Test and Verify System Behavior

Once the system has completed boot, you can run the checks below. For each test, look for the corresponding activity on the **CM33 UART console**.

1. **Power-on boot verification**
	- Power up the board with the generated binaries flashed.
	- Verify that the CM33 UART shows the boot banners, the watchdog monitor startup log, the A35 bring-up sequence, and any project-specific UserApp activity you enabled.

2. **Cold reset from Linux**
	- On Linux, run:
	  ```bash
	  reboot
	  ```
	- Expected behavior: the full platform reboots and the CM33 UART shows the MCUboot and TF-M banners again.

3. **Warm reset from Linux**
	- On Linux, run:
	  ```bash
	  echo warm >> /sys/kernel/reboot/mode
	  reboot
	  ```
	- Expected behavior: the CM33 UART shows the SCMI warm-reset notification and the RemoteProc stop/start sequence.

4. **Template application hook verification**
	- Build and run the template with your project-side UserApp behavior enabled.
	- Verify that the board-level activity you added to the template, such as LED toggling or periodic logging, continues once the core stack has completed initialization.

---

## Error Behaviors

If an error occurs during initialization or system configuration at runtime, **LED3 will blink at a 100 ms interval** to indicate the error state.

---

## How to Flash Binaries

1. **Build/Compile the Project**:
	Follow the [Build Procedure](#build-procedure) to generate the required binaries.

2. **Copy and Rename Generated Binaries**:
	Before copying, **rename the generated binaries** (`bl2.stm32`, `ddr_phy_signed.bin`, and `tfm_s_ns_signed.bin`) according to the names specified in the **Required Binaries** section under [Reference Use Cases for MP25-EV1 Board (Template_StarterApp Project)](#reference-use-cases-for-mp25-ev1-board-template_starterapp-project).

	Then, navigate to the `bin/` folder and copy the renamed binaries to the following paths:
	```
	<OSTL-IMAGE-PATH>/images/stm32mp2-m33td/arm-trusted-firmware-m/bl2
	```
	- Place the renamed `bl2` binary in this directory.
	```
	<OSTL-IMAGE-PATH>/images/stm32mp2-m33td/m33-firmware
	```
	- Place the renamed `ddr_phy` and `tfm-starterapp` binaries in this directory.

3. **Modify Flashlayout (TSV)**:
	Select the relevant TSV file under `<OSTL-IMAGE-PATH>/images/stm32mp2-m33td/flashlayout_st-image-weston/optee` as per specific dev mode (see [Flash Layouts for MP25-EV1 Board](#flash-layouts-for-mp25-ev1-board)).
	Replace the existing TF-M NS signed binary name with the name of your renamed `tfm-starterapp-..._s_ns_Signed.bin` that corresponds to your chosen boot mode.

4. **Connect the Device**:
	Use a Type-C cable to connect the device to the Type-C connector.

5. **Flash the Image**:
	Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/) for detailed flashing instructions.

6. **Perform a Power-On Reset**:
	After flashing, perform a power-on reset to complete the process.

---

## Different Boot Device Modes for MP25-EV1 Board

The MP25-EV1 board supports the following boot device modes:

1. **emmc_emmc**:
	- MCUBOOT/TFM(S/NS) -> eMMC
	- OSTL -> eMMC

2. **nor_emmc**:
	- MCUBOOT/TFM(S/NS) -> Serial NOR
	- OSTL -> eMMC

3. **nor_nor_sdcard**:
	- MCUBOOT/TFM(S/NS) -> Serial NOR
	- TF-A/FIP -> Serial NOR
	- OSTL filesystem -> SD card

4. **nor_sdcard**:
	- MCUBOOT/TFM(S/NS) -> Serial NOR
	- OSTL -> SD card

5. **sdcard_sdcard**:
	- MCUBOOT/TFM(S/NS) -> SD card
	- OSTL -> SD card

---

## Flash Layouts for MP25-EV1 Board

For the MP25-EV1 board, five TSV flavors are provided to flash under the M33TDCID profile:

- `FlashLayout_emmc_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`
- `FlashLayout_nor-emmc_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`
- `FlashLayout_nor-nor-sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`
- `FlashLayout_sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`
- `FlashLayout_nor-sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`

**Note**: Each TSV requires three sets of binaries. Refer to the [Reference Use Cases for MP25-EV1 Board (Template_StarterApp Project)](#reference-use-cases-for-mp25-ev1-board-template_starterapp-project) section for details.

---

## How to Generate Binaries for Different Boot Modes

1. **Configure and Build TFM Secure**:
	Use specific CMake options defined for each boot mode as described in the **Required CMake Options** section under [Reference Use Cases for MP25-EV1 Board (Template_StarterApp Project)](#reference-use-cases-for-mp25-ev1-board-template_starterapp-project).

2. **Build Template_StarterApp Project**:
	Compile the Template_StarterApp project after building the TFM secure binaries.

3. **Rename Generated Binaries**:
	Rename the binaries in the `bin/` folder to match the required binaries for the specific TSV as described in the **Required Binaries** section under [Reference Use Cases for MP25-EV1 Board (Template_StarterApp Project)](#reference-use-cases-for-mp25-ev1-board-template_starterapp-project).

---

## Reference Use Cases for MP25-EV1 Board (Template_StarterApp Project)

### 1. FlashLayout_emmc_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv (emmc_emmc Boot Mode)

**Required Binaries**:
- `bl2-stm32mp257f-ev1-cm33tdcid-ostl-emmc.stm32`
- `ddr_phy-stm32mp257f-ev1-cm33tdcid-ostl-emmc_Signed.bin`
- `tfm-starterapp-stm32mp257f-ev1-cm33tdcid-ostl-emmc-emmc_s_ns_Signed.bin`

**Required CMake Options**:
```bash
-DSTM32_BOOT_DEV=sdmmc2  # eMMC
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-emmc-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-emmc-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts
```

---

### 2. FlashLayout_nor-emmc_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv (nor_emmc Boot Mode)

**Required Binaries**:
- `bl2-stm32mp257f-ev1-cm33tdcid-ostl-nor.stm32`
- `ddr_phy-stm32mp257f-ev1-cm33tdcid-ostl-nor_Signed.bin`
- `tfm-starterapp-stm32mp257f-ev1-cm33tdcid-ostl-nor-emmc_s_ns_Signed.bin`

**Required CMake Options**:
```bash
-DSTM32_BOOT_DEV=ospi  # Serial NOR
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-snor-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-emmc-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts
```

---

### 3. FlashLayout_nor-nor-sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv (nor_nor_sdcard Boot Mode)

**Required Binaries**:
- `bl2-stm32mp257f-ev1-cm33tdcid-ostl-nor.stm32`
- `ddr_phy-stm32mp257f-ev1-cm33tdcid-ostl-nor_Signed.bin`
- `tfm-starterapp-stm32mp257f-ev1-cm33tdcid-ostl-nor-nor_s_ns_Signed.bin`

**Required CMake Options**:
```bash
-DSTM32_BOOT_DEV=ospi  # Serial NOR
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-snor-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-snor-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts
-DTFM_PARTITION_PROTECTED_STORAGE=OFF
```

---

### 4. FlashLayout_nor-sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv (nor_sdcard Boot Mode)

**Required Binaries**:
- `bl2-stm32mp257f-ev1-cm33tdcid-ostl-nor.stm32`
- `ddr_phy-stm32mp257f-ev1-cm33tdcid-ostl-nor_Signed.bin`
- `tfm-starterapp-stm32mp257f-ev1-cm33tdcid-ostl-nor-sdcard_s_ns_Signed.bin`

**Required CMake Options**:
```bash
-DSTM32_BOOT_DEV=ospi  # Serial NOR
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-snor-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts
```

---

### 5. FlashLayout_sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv (sdcard_sdcard Boot Mode)

**Required Binaries**:
- `bl2-stm32mp257f-ev1-cm33tdcid-ostl-sdcard.stm32`
- `ddr_phy-stm32mp257f-ev1-cm33tdcid-ostl-sdcard_Signed.bin`
- `tfm-starterapp-stm32mp257f-ev1-cm33tdcid-ostl-sdcard-sdcard_s_ns_Signed.bin`

**Required CMake Options**:
```bash
-DSTM32_BOOT_DEV=sdmmc1  # SD card
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts
```

---

## Integration Intent

Use this template when creating a new M33TD project that should inherit the common NSAppCore services first and then selectively enable optional features such as OpenAMP, LowPowerMgr, Button Monitor, or Display according to the final product profile.
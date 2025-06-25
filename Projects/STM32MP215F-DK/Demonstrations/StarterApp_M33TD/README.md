# StarterApp_M33TD Demonstration Application

---

## Application Description

The StarterApp_M33TD demonstration is to highlight the capabilities of the STM32MP2 platform for M33TDCID profile, showcasing the M33 as the primary CPU and the A35 as a high-performance coprocessor. This application emphasizes the ability to achieve a fast Cortex-M boot, bypassing the Cortex-A and the associated OpenSTLinux solution, which typically requires several seconds to initialize. By leveraging this approach, the system can quickly execute critical tasks in the non-secure environment while maintaining flexibility for high-performance operations on the A35.

---

## Key Features

- **FreeRTOS Multitasking**: The application is built on FreeRTOS, enabling multitasking in the non-secure environment.
- **LED Blinking**: A task to blink an LED every second.
- **A35 Coprocessor Management**: A dedicated task to manage the A35 core.
- **Interrupt Handling**: Configures the EXTI line to receive interrupts on NVIC line 4 of the M33 on the event IWDG RST.

---

## Purpose

This is a FreeRTOS-based multitask application running in the NS processing environment, while the TFM secure application operates in the secure processing environment. The secure application acts as a client to process secure services requested by both the A35 and M33 NS.

1. **UserApp Task**:
   - A user application task that blinks an LED every second to indicate the M33 system is functioning correctly.

2. **RemoteProc Task**:
   - Manages the A35 coprocessor, including:
     - Starting the A35 automatically during boot by requesting a TFM secure service.
     - Retrieving the A35 CPU status through a secure service.
     - Restarting the A35 in case of a crash, using NVIC interruptions and EXTI line configuration.


## Prerequisite Hardware & Software Environment Setup

- **Trusted Firmware-M**: The source code must be installed under the `Middlewares/Third_Party` directory with the path `Middlewares/Third_Party/trusted-firmware-m`. Ensure the correct version is used as described in the STM32 MPU release note.
- **Supported Devices**: This example runs on STM32MP21xx devices and has been tested with the STMicroelectronics STM32MP215F-DK board. It can be tailored to other supported devices and development boards.
- **ST-Link Connection**: Connect the ST-Link cable to the PC USB port to display traces.

### Software Versions
- **Trusted Firmware-M (TFM)**: Refer to the [Trusted Firmware-M wiki](https://wiki.st.com/stm32mpu/wiki/Category:Trusted_Firmware-M) for recommended versions and integration details.
- **External Device Tree (externalDT)**: See the [External Device Tree wiki](https://wiki.st.com/stm32mpu/wiki/External_device_tree) for guidance on obtaining and using external DT sources.
- **STM32CubeIDE**: For supported IDE versions and ecosystem information, refer the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/).
  
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
   cmake -B config_default -G"Unix Makefiles" -DTFM_PLATFORM=stm/stm32mp215f_dk -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake -DSTM32_BOOT_DEV=sdmmc1 -DTFM_PROFILE=profile_medium -DSTM32_M33TDCID=ON -DCMAKE_BUILD_TYPE=Relwithdebinfo -DNS=OFF -DDTS_EXT_DIR=<EXT_DT_DIR> -DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts -DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts 
   -DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-ns.dts 
   ```
3. Build the project:
   ```bash
   cmake --build config_default -- install
   ```

**Note**: The external DT repository is placed in the `Utilities` directory.

---

### Non-Secure M33 Firmware Build (StarterApp_M33TD Application)
1. Navigate to `Firmware/Projects/STM32MP215F-DK/Demonstrations/StarterApp_M33TD`.
2. Run the following command:
   ```bash
      cmake -G"Unix Makefiles" -B build -DTFM_BUILD_DIR=<TFM_BUILD_DIRECTORY> -DREMOTE_PROC_AUTO_START=ON
   ```
      - If `TFM_BUILD_DIR` is not specified, the default path `Firmware/Middlewares/Third_Party/trusted-firmware-m/config_default` will be used, assuming the TFM Secure Build step is completed.
      - By default:
        - `REMOTE_PROC_AUTO_START` is set to `1` for StarterApp_M33TD unless explicitly provided

3. Build the project:
   ```bash
   make -C build all
   ```

---

## To Build Native STM32CubeIDE Project

### Project Structure
```
StarterApp_M33TD
├── StarterApp_M33TD_CM33
│   ├── StarterApp_M33TD_CM33_NonSecure (Non-Secure STM32CubeIDE M33 project)
│   └── StarterApp_M33TD_CM33_trusted-firmware-m (Secure CMake project)
```

**Note**: Refer to [this wiki](https://wiki.st.com/stm32mpu/wiki/How_to_create_an_M33-TD_boot_project_using_STM32CubeIDE#) for instructions on importing a CMake project.

### Build Procedure

#### Secure TFM Firmware Build
1. Configure the CMake build options:
    - Navigate to `StarterApp_M33TD_CM33_trusted-firmware-m` and update the CMake settings:
      ```
      -DDEBUG_AUTHENTICATION=FULL  # Enabled for Debug Purpose, Default: this option is removed
      -DTFM_PLATFORM=stm/stm32mp215f_dk
      -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake
      -DSTM32_BOOT_DEV=sdmmc1  # Building TFM for "sdcard_sdcard" bootdevice mode
      -DTFM_PROFILE=profile_medium
      -DSTM32_M33TDCID=ON
      -DCMAKE_BUILD_TYPE=Relwithdebinfo
      -DNS=OFF
      -DDTS_EXT_DIR=../../../../../../../../../Firmware/Utilities/dt-stm32mp
      -DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts  # External DT file for sdcard_sdcard bootdevice mode 
      -DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts          # External DT file for sdcard_sdcard bootdevice mode 
      -DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-ns.dts               # External DT file common for all bootdevice modes
      ```
2. Configure the TFM CMake project:
    - Right-click on `StarterApp_M33TD_CM33_trusted-firmware-m` -> `CMake Configure`.
3. Build the TFM CMake project:
    - Right-click on `StarterApp_M33TD_CM33_trusted-firmware-m` -> `Build Project`.

#### Non-Secure STM32CubeIDE Project Build
1. Build the project:
    - Select the build configuration as per the TFM version:
      - Choose `CM33TDCID_m33_ns_tfm_s_sign`.
    - Right-click on `StarterApp_M33TD_CM33_NonSecure` -> `Build Project`.

---

## Output

- The generated binaries will be located in the `bin` folder:
```
   cd bin/
   ```
   - `StarterApp_M33TD_CM33_NonSecure.bin`
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
[INF] Loading gpt header

[INF] welcome to MCUboot: v2.1.0-stm32mp-r1-rc5
[INF] cpu: STM32MP215FAN Rev.A
[INF] board: stm32mp215f dk
[INF] dts: stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts
[INF] boot device: sdmmc1
[INF] mcu sysclk: 300000000
[INF] Starting bootloader
[INF] ***************************

[INF] Provisioning bypassed, if your OTP are not set.

[INF] Default values will be used.

[INF] DUMMY_PROVISIONING is not suitable for production!

[INF] This device is NOT SECURE

[INF] ***************************

[WRN] This device was provisioned with dummy keys.

[WRN] This device is NOT SECURE

[WRN] NV_MM_COUNTER_INIT is not suitable for production! This device is NOT SECURE

[INF] Primary   slot: version=0.1.0+0
[INF] Image 1 Secondary slot: Image not found
[INF] Image 1 RAM loading to 0xe065000 is succeeded.
[INF] Image 1 loaded from the primary slot
[INF] BL2: image 1, enable DDR-FW
[INF] Primary   slot: version=2.1.0+0
[INF] Image 0 Secondary slot: Image not found
[INF] Image 0 RAM loading to 0x80000000 is succeeded.
[INF] Image 0 loaded from the primary slot
[INF] Bootloader chainload address offset: 0x104400
[INF] Jumping to the first image slot
[INF] welcome to TF-M: v2.1.0-stm32mp-r1-rc5
[INF] board: stm32mp215f dk
[INF] dts: stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts
[INF] ***************************
[INF] Provisioning bypassed, if your OTP are not set.
[INF] Default values will be used.
[INF] DUMMY_PROVISIONING is not suitable for production!
[INF] This device is NOT SECURE
[INF] ***************************
[WAR] This device was provisioned with dummy keys.
[WAR] This device is NOT SECURE
[NS] [INF] Non-Secure system starting...
[NS] [INF] STM32Cube FW version: v1.2.0-rc8
[NS] [RemoteProc] [INF] starting copro cpu@0... done
[NS] [RemoteProc] [CRASH] Crash detected on cpu0. Attempting recovery...
[NS] [RemoteProc] [INF] stopping copro cpu@0...
[NS] [RemoteProc] [INF] starting copro cpu@0... done
[NS] [RemoteProc] [INF] Crash recovery sequence completed
```

### Understanding TestApp Logs

This log demonstrates the full boot process, provisioning status, image loading, and remote processor management, including crash detection and recovery.  
> **Note:** The `[RemoteProc] starting copro cpu@0... done` message confirms that the A35 coprocessor was started automatically at boot. This behavior is enabled by the `REMOTE_PROC_AUTO_START=ON` CMake option, as described in the [Non-Secure M33 Firmware Build (StarterApp_M33TD Application)](#non-secure-m33-firmware-build-starterapp_m33td-application) section.  
To trigger a crash and observe the recovery sequence as shown in the log, you can run the following command on the Linux console (as root):

```bash
sync; sleep 2; sync; echo c > /proc/sysrq-trigger
```

This command forces a kernel crash, allowing you to verify the crash handling and recovery features implemented in the StarterApp_M33TD application.

The log also reflects the specific firmware and configuration used for the device boot:
- **CubeFW Version**: The STM32Cube firmware version is shown (e.g., `STM32Cube FW version: v1.2.0-rc8`).
- **TFM Version**: The Trusted Firmware-M version is indicated (e.g., `welcome to TF-M: v2.1.0-stm32mp-r1-rc5`).
- **Device Tree Files**: The log lists the external device tree files used for the build and boot (e.g., `dts: stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts` and `dts: stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts`).

To verify that the correct configuration is reflected, compare the device tree filenames and firmware versions in the log with the CMake options and external DTS files specified during the TFM secure build (see [How to Generate Binaries for Different Boot Modes](#how-to-generate-binaries-for-different-boot-modes)). This ensures that the intended configuration and external DTS references are being used for your specific boot mode and hardware.

## Error Behaviors
If an error occurs during initialization or system configuration at runtime, **LED3 will blink at a 100 ms interval** to indicate the error state.

## Assumptions


## Known Limitations


## Keywords
Security, TFM, Secure, SD Card, Non-Secure

## How to Flash Binaries

1. **Build/Compile the Project**  
  Follow the [Build Procedure](#build-procedure) to generate the required binaries.

2. **Copy and Rename Generated Binaries**  
   Before copying, **rename the generated binaries** (`bl2.stm32`, `ddr_phy_signed.bin`, and `tfm_s_ns_signed.bin`) according to the names specified in the **Required Binaries** section of the relevant flash layout under [Reference Use Cases for MP21-DK Board (StarterApp Project)](#reference-use-cases-for-mp21-dk-board-starterapp-project).  
   Then, navigate to the `bin/` folder and copy the renamed binaries to the following paths:
   ```
   <OSTL-IMAGE-PATH>/images/stm32mp2-m33td/arm-trusted-firmware-m
   ```
   - Place the renamed `bl2` and `ddr_phy_signed` binaries in this directory.
   ```
   <OSTL-IMAGE-PATH>/images/stm32mp2-m33td/arm-trusted-firmware-m-cube
   ```
   - Place the renamed `tfm_s_ns_signed` binary in this directory.

3. **Modify Flashlayout (TSV)**  
   Select the relevant TSV file under `<OSTL-IMAGE-PATH>/images/stm32mp2-m33td/flashlayout_st-image-weston/optee` as per specific dev mode (see [Flash Layouts for MP21-DK Board](#flash-layouts-for-mp21-dk-board)).
   And then modify the TSV file, by replacing the existing `tfm_s_ns_signed` binary name with the name of your newly generated and renamed `tfm_s_ns_signed` binary that corresponds to your chosen boot mode.

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
   - MCUBOOT/TFM(S/NS) → SD card  
   - OSTL → SD card  

---

## Flash Layouts for MP21-DK Board

For the MP21-DK board, only one TSV flavor is provided to flash under the M33TDCID profile:

- `FlashLayout_sdcard_stm32mp215f-dk-cm33tdcid-ostl-optee.tsv`

**Note**: Each TSV requires three sets of binaries. Refer to the [Reference Use Cases for MP21-DK Board (StarterApp Project)](#reference-use-cases-for-mp21-dk-board-starterapp-project) section for details.

---

## How to Generate Binaries for Different Boot Modes

1. **Configure and Build TFM Secure**:  
  Use specific CMake options defined for each boot mode as described in the **Required CMake Options** section under [Reference Use Cases for MP21-DK Board (StarterApp Project)](#reference-use-cases-for-mp21-dk-board-starterapp-project).  

2. **Build StarterApp Project**:  
   Compile the StarterApp project after building the TFM secure binaries.  

3. **Rename Generated Binaries**:  
   Rename the binaries in the `bin/` folder to match the required binaries for the specific TSV as described in the **Required Binaries** section under [Reference Use Cases for MP21-DK Board (StarterApp Project)](#reference-use-cases-for-mp21-dk-board-starterapp-project).  

---

## Reference Use Cases for MP21-DK Board (StarterApp Project)

### 1. FlashLayout_sdcard_stm32mp215f-dk-cm33tdcid-ostl-optee.tsv (sdcard_sdcard Boot Mode)

**Required Binaries**:  
- `bl2-stm32mp215f-dk-cm33tdcid-ostl-sdcard.stm32`  
- `ddr_phy_signed-stm32mp215f-dk-cm33tdcid-ostl-sdcard.bin`  
- `tfm-starterapp-stm32mp215f-dk-cm33tdcid-ostl-sdcard-sdcard_s_ns_signed.bin`  

**Required CMake Options**:  
```bash
-DSTM32_BOOT_DEV=sdmmc1  # SD card
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp215f-dk-cm33tdcid-ostl-sdcard-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-sdcard-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp215f-dk-cm33tdcid-ostl-ns.dts
```

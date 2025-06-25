# TestApp_M33TD Demonstration Application

---

## Application Description

The TestApp_M33TD demonstration showcases the capabilities of the STM32MP2 platform for the M33TDCID profile, highlighting the Cortex-M33 as the primary CPU and the Cortex-A35 as a high-performance coprocessor. This application demonstrates the execution of HAL M33 examples in the M33TDCID profile while managing critical tasks such as RemoteProc and UserApp blinking LED. 

In addition to these tasks, the application introduces a dedicated **TestApp Task** to run a variety of tests sequentially, showcasing the multitasking capabilities of FreeRTOS in the non-secure (NS) environment. These tests validate the functionality of the M33 examples under the M33TDCID profile, with each test categorized as either automatic or manual depending on external dependencies.

---

## Key Features

- **FreeRTOS Multitasking**: The application is built on FreeRTOS, enabling multitasking in the non-secure environment.
- **UserApp Task**: A task to blink an LED every second.
- **RemoteProc Task**: A dedicated task to manage the A35 core.
- **TestApp Task**: A task to run various tests sequentially.

---

## Purpose

The purpose of this demonstration is to validate the functionality of HAL M33 examples in the M33TDCID profile while showcasing the multitasking capabilities of FreeRTOS in the non-secure environment. The application achieves this through the following tasks:

1. **UserApp Task**:
   - A user application task that blinks an LED every second to indicate the M33 system is functioning correctly.

2. **RemoteProc Task**:
   - Manages the A35 coprocessor, including:
     - Starting the A35 on demand (not started by default on boot) by requesting a TFM secure service.
     - Retrieving the A35 CPU status through a secure service.
     - Restarting the A35 in case of a crash, using NVIC interruptions and EXTI line configuration.

3. **TestApp Task**:
   - Runs a variety of tests sequentially to validate the functionality of HAL M33 examples:
     - **Automatic Tests**:
       - **ADC**: Internal voltage conversion using ADC.
       - **TIMERS**: Synchronization of multiple timers.
     - **Manual Tests**:
       - **I2C**: Control of a Low Power I2C Display.
       - **I3C**: Data transmission/reception as a controller (Master).
       - **SPI**: Data transmission/reception as a controller (Master).

---

## Prerequisite Hardware & Software Environment Setup

- **Trusted Firmware-M**: The source code must be installed under the `Middlewares/Third_Party` directory with the path `Middlewares/Third_Party/trusted-firmware-m`. Ensure the correct version is used as described in the STM32 MPU release note.
- **Supported Devices**: This example runs on STM32MP25xx devices and has been tested with the STMicroelectronics STM32MP257F-EV1 board. It can be tailored to other supported devices and development boards.
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
   cmake -B config_default -G"Unix Makefiles" -DTFM_PLATFORM=stm/stm32mp257f_ev1 -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake -DSTM32_BOOT_DEV=sdmmc1 -DTFM_PROFILE=profile_medium -DSTM32_M33TDCID=ON -DCMAKE_BUILD_TYPE=Relwithdebinfo -DNS=OFF -DDTS_EXT_DIR=<EXT_DT_DIR> -DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-bl2.dts -DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-s.dts 
   -DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts 
   ```
3. Build the project:
   ```bash
   cmake --build config_default -- install
   ```

**Note**: The external DT repository is placed in the `Utilities` directory.

---

### Non-Secure M33 Firmware Build (TestApp_M33TD Application)
1. Navigate to `Firmware/Projects/STM32MP257F-EV1/Demonstrations/TestApp_M33TD`.
2. Run the following command:
  ```bash
  cmake -G"Unix Makefiles" -B build -DTFM_BUILD_DIR=<TFM_BUILD_DIRECTORY> -DENABLE_AUTO_TEST=ON
  ```

  - If `TFM_BUILD_DIR` is not specified, the default path `Firmware/Middlewares/Third_Party/trusted-firmware-m/config_default` will be used, assuming the TFM Secure Build step is completed.
  - By default: 
    - `REMOTE_PROC_AUTO_START` is set to `0` for TestApp_M33TD unless explicitly provided.
    - `ENABLE_AUTO_TEST` is set to `0` for TestApp_M33TD unless explicitly provided.
    **Note**: For more detailed build configuration for TestApp execution, refer to the [TestApp Task Configuration](#testapp-task-configuration) section.
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
│   ├── TestApp_M33TD_CM33_NonSecure (Non-Secure STM32CubeIDE M33 project)
│   └── TestApp_M33TD_CM33_trusted-firmware-m (Secure CMake project)
```

**Note**: Refer to [this wiki](https://wiki.st.com/stm32mpu/wiki/How_to_create_an_M33-TD_boot_project_using_STM32CubeIDE#) for instructions on importing a CMake project.

### Build Procedure

#### Secure TFM Firmware Build
1. Configure the CMake build options:
    - Navigate to `TestApp_M33TD_CM33_trusted-firmware-m` and update the CMake settings:
      ```
      -DDEBUG_AUTHENTICATION=FULL  # Enabled for Debug Purpose, Default: this option is removed
      -DTFM_PLATFORM=stm/stm32mp257f_ev1
      -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake
      -DSTM32_BOOT_DEV=sdmmc1  # Building TFM for "sdcard_sdcard" bootdevice mode
      -DTFM_PROFILE=profile_medium
      -DSTM32_M33TDCID=ON
      -DCMAKE_BUILD_TYPE=Relwithdebinfo
      -DNS=OFF
      -DDTS_EXT_DIR=../../../../../../../../../Firmware/Utilities/dt-stm32mp
      -DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-bl2.dts  # External DT file for sdcard_sdcard bootdevice mode 
      -DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-s.dts          # External DT file for sdcard_sdcard bootdevice mode 
      -DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts               # External DT file common for all bootdevice modes
      ```
2. Configure the TFM CMake project:
    - Right-click on `TestApp_M33TD_CM33_trusted-firmware-m` -> `CMake Configure`.
3. Build the TFM CMake project:
    - Right-click on `TestApp_M33TD_CM33_trusted-firmware-m` -> `Build Project`.

#### Non-Secure STM32CubeIDE Project Build
1. Build the project:
    - Select the build configuration as per the TFM version:
      - Choose `CM33TDCID_m33_ns_tfm_s_sign`.
    - Right-click on `TestApp_M33TD_CM33_NonSecure` -> `Build Project`.

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
  - **AUTO** tests will be fully executed, including all steps (AcquireResource → Init → Run → Deinit → ReleaseResource).
  - **MANUAL** tests will be partially executed, performing only the resource acquisition and release steps (AcquireResource → ReleaseResource).
- **Purpose**:
  - This scenario validates the correct runtime configuration settings required for specific M33 examples in the M33TDCID profile.

#### **Scenario 2: ENABLE_AUTO_TEST is OFF (Set to 0)**
- **Behavior**:
  - The TestApp will execute tests fully based on **User Button 2** actions.
  - Both **AUTO** and **MANUAL** tests will be executed completely (AcquireResource → Init → Run → Deinit → ReleaseResource).

### How to Run Manual Tests

#### I3C Test
- Ensure the target I2C board is ready before executing the I3C controller test.
- Refer to the examples provided at `Firmware/Projects/STM32MP257F-EV1/Examples/I3C` to set up the target I2C board.

#### SPI Test
- Ensure the slave SPI board is ready before executing the SPI Master test.
- Refer to the examples provided at `Firmware/Projects/STM32MP257F-EV1/Examples/SPI` to set up the slave board for testing.

#### I2C Test
- Ensure the I2C Low Power Display is connected to the board.
- Refer to the example at `Firmware/Projects/STM32MP257F-EV1/Demonstrations/M33TD_DEMO/DisplayDemo_M33TD` to set up the display.

---

### Example CMake Options
To control the test execution behavior, use the following CMake options during the build process:
```bash
-ENABLE_AUTO_TEST=ON   # Enables automatic execution of Test Sequence.
-ENABLE_AUTO_TEST=OFF  # Disables automatic execution; tests are triggered manually.
```

### Example CubeIDE Preprocessor Definitions
To control the test execution behavior in CubeIDE, define the following preprocessor macros in project settings under compiler preprocessor configuration:
```c
ENABLE_AUTO_TEST 1  // Enables automatic execution of Test Sequence.
ENABLE_AUTO_TEST 0  // Disables automatic execution; tests are triggered manually.
```

---

This section ensures that developers can configure and control the execution of the TestApp task based on their requirements, providing flexibility for both automated and manual testing scenarios.

---

#### UART Console Output

Below is a sample UART log output from the CM33 during the boot and runtime sequence:

```
[INF] Loading gpt header

[INF] welcome to MCUboot: v2.1.0-stm32mp-r1-rc5
[INF] cpu: STM32MP257FAI Rev.Y
[INF] board: stm32mp257f eval1
[INF] board ID: MB1936 Var1.0 Rev.D-01
[INF] dts: stm32mp257f-ev1-cm33tdcid-ostl-sdcard-bl2.dts
[INF] boot device: sdmmc1
[INF] mcu sysclk: 400000000
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
[INF] Image 1 RAM loading to 0xe060000 is succeeded.
[INF] Image 1 loaded from the primary slot
[INF] BL2: image 1, enable DDR-FW
[INF] Primary   slot: version=2.1.0+0
[INF] Image 0 Secondary slot: Image not found
[INF] Image 0 RAM loading to 0x80000000 is succeeded.
[INF] Image 0 loaded from the primary slot
[INF] Bootloader chainload address offset: 0x104400
[INF] Jumping to the first image slot
[INF] Enable Macronix quad support
[INF] welcome to TF-M: v2.1.0-stm32mp-r1-rc5
[INF] board: stm32mp257f eval1
[INF] dts: stm32mp257f-ev1-cm33tdcid-ostl-sdcard-s.dts
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
[NS] [TestApp] [INF] Acquiring resources for test: TIMERS
[NS] [TestApp] [INF] Running test: TIMERS
[NS] [TestApp] [INF] Test TIMERS: PASSED
[NS] [TestApp] [INF] Releasing resources for test: TIMERS
[NS] [TestApp] [INF] Acquiring resources for test: I2C
[NS] [TestApp] [INF] Skipping manual test in auto Test mode: I2C
[NS] [TestApp] [INF] Releasing resources for test: I2C
[NS] [TestApp] [INF] Acquiring resources for test: SPI
[NS] [TestApp] [INF] Skipping manual test in auto Test mode: SPI
[NS] [TestApp] [INF] Releasing resources for test: SPI
[NS] [TestApp] [INF] Acquiring resources for test: ADC
[NS] [TestApp] [INF] Running test: ADC
[NS] [TestApp] [INF] Test ADC: PASSED
[NS] [TestApp] [INF] Releasing resources for test: ADC
[NS] [TestApp] [INF] Acquiring resources for test: I3C
[NS] [TestApp] [INF] Skipping manual test in auto Test mode: I3C
[NS] [TestApp] [INF] Releasing resources for test: I3C
[NS] [TestApp] [INF] All tests completed.
```

### Understanding TestApp Logs

The TestApp logs provide detailed, step-by-step insight into the execution of each test within the TestApp task. When running in automatic mode (`ENABLE_AUTO_TEST=ON`), the TestApp sequentially acquires resources, initializes, runs, and deinitializes each test categorized as **AUTO**, while **MANUAL** tests are acknowledged but skipped unless user interaction is required. These logs help developers verify that each test is executed in the correct order, resources are properly managed, and the overall system behavior matches the configuration described in [TestApp Task Configuration](#testapp-task-configuration). The log output is essential for debugging, validating test results, and ensuring the correct operation of the multitasking environment.

The log also reflects the specific firmware and configuration used for the device boot:
- **CubeFW Version**: The STM32Cube firmware version is shown (e.g., `STM32Cube FW version: v1.2.0-rc8`).
- **TFM Version**: The Trusted Firmware-M version is indicated (e.g., `welcome to TF-M: v2.1.0-stm32mp-r1-rc5`).
- **Device Tree Files**: The log lists the external device tree files used for the build and boot (e.g., `dts: stm32mp257f-ev1-cm33tdcid-ostl-sdcard-bl2.dts` and `dts: stm32mp257f-ev1-cm33tdcid-ostl-sdcard-s.dts`).

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
   Before copying, **rename the generated binaries** (`bl2.stm32`, `ddr_phy_signed.bin`, and `tfm_s_ns_signed.bin`) according to the names specified in the **Required Binaries** section of the relevant flash layout under [Reference Use Cases for MP25-EV1 Board (TestApp Project)](#reference-use-cases-for-mp25-ev1-board-testapp-project).  
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
   Select the relevant TSV file under `<OSTL-IMAGE-PATH>/images/stm32mp2-m33td/flashlayout_st-image-weston/optee` as per specific dev mode (see [Flash Layouts for MP25-EV1 Board](#flash-layouts-for-mp25-ev1-board)).
   And then modify the TSV file, by replacing the existing `tfm_s_ns_signed` binary name with the name of your newly generated and renamed `tfm_s_ns_signed` binary that corresponds to your chosen boot mode.

4. **Connect the Device**  
  Use a Type-C cable to connect the device to the Type-C connector.

5. **Flash the Image**  
  Refer to the [STM32 MPU wiki](https://wiki.st.com/stm32mpu/wiki/) for detailed flashing instructions.

6. **Perform a Power-On Reset**  
  After flashing, perform a power-on reset to complete the process.

---

## Different Boot Device Modes for MP25-EV1 Board

The MP25-EV1 board supports the following boot device modes:

1. **emmc_emmc**:  
   - MCUBOOT/TFM(S/NS) → eMMC  
   - OSTL → eMMC  

2. **nor_emmc**:  
   - MCUBOOT/TFM(S/NS) → Serial NOR  
   - OSTL → eMMC  

3. **nor_nor_sdcard**:  
   - MCUBOOT/TFM(S/NS) → Serial NOR  
   - TF-A/FIP → Serial NOR  
   - OSTL filesystem → SD card  

4. **nor_sdcard**:  
   - MCUBOOT/TFM(S/NS) → Serial NOR  
   - OSTL → SD card  

5. **sdcard_sdcard**:  
   - MCUBOOT/TFM(S/NS) → SD card  
   - OSTL → SD card  

---

## Flash Layouts for MP25-EV1 Board

For the MP25-EV1 board, five TSV flavors are provided to flash under the M33TDCID profile:

- `FlashLayout_emmc_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`
- `FlashLayout_nor-emmc_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`
- `FlashLayout_nor-nor-sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`
- `FlashLayout_sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`
- `FlashLayout_nor-sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv`

**Note**: Each TSV requires three sets of binaries. Refer to the [Reference Use Cases for MP25-EV1 Board (TestApp Project)](#reference-use-cases-for-mp25-ev1-board-testapp-project) section for details.

---

## How to Generate Binaries for Different Boot Modes

1. **Configure and Build TFM Secure**:  
  Use specific CMake options defined for each boot mode as described in the **Required CMake Options** section under [Reference Use Cases for MP25-EV1 Board (TestApp Project)](#reference-use-cases-for-mp25-ev1-board-testapp-project).  

2. **Build TestApp Project**:  
   Compile the TestApp project after building the TFM secure binaries.  

3. **Rename Generated Binaries**:  
   Rename the binaries in the `bin/` folder to match the required binaries for the specific TSV as described in the **Required Binaries** section under [Reference Use Cases for MP25-EV1 Board (TestApp Project)](#reference-use-cases-for-mp25-ev1-board-testapp-project).  

---

## Reference Use Cases for MP25-EV1 Board (TestApp Project)

### 1. FlashLayout_emmc_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv (emmc_emmc Boot Mode)

**Required Binaries**:  
- `bl2-stm32mp257f-ev1-cm33tdcid-ostl-emmc.stm32`  
- `ddr_phy_signed-stm32mp257f-ev1-cm33tdcid-ostl-emmc.bin`  
- `tfm-testapp-stm32mp257f-ev1-cm33tdcid-ostl-emmc-emmc_s_ns_signed.bin`  

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
- `ddr_phy_signed-stm32mp257f-ev1-cm33tdcid-ostl-nor.bin`  
- `tfm-testapp-stm32mp257f-ev1-cm33tdcid-ostl-nor-emmc_s_ns_signed.bin`  

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
- `ddr_phy_signed-stm32mp257f-ev1-cm33tdcid-ostl-nor.bin`  
- `tfm-testapp-stm32mp257f-ev1-cm33tdcid-ostl-nor-nor_s_ns_signed.bin`  

**Required CMake Options**:  
```bash
-DSTM32_BOOT_DEV=ospi  # Serial NOR
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-snor-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-snor-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts
-DTFM_PARTITION_PROTECTED_STORAGE=OFF     #For this dev mode , Protected Storage feature needs to be disabled 
```

---

### 4. FlashLayout_nor-sdcard_stm32mp257f-ev1-cm33tdcid-ostl-optee.tsv (nor_sdcard Boot Mode)

**Required Binaries**:  
- `bl2-stm32mp257f-ev1-cm33tdcid-ostl-nor.stm32`  
- `ddr_phy_signed-stm32mp257f-ev1-cm33tdcid-ostl-nor.bin`  
- `tfm-testapp-stm32mp257f-ev1-cm33tdcid-ostl-nor-sdcard_s_ns_signed.bin`  

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
- `ddr_phy_signed-stm32mp257f-ev1-cm33tdcid-ostl-sdcard.bin`  
- `tfm-testapp-stm32mp257f-ev1-cm33tdcid-ostl-sdcard-sdcard_s_ns_signed.bin`  

**Required CMake Options**:  
```bash
-DSTM32_BOOT_DEV=sdmmc1  # SD card
-DDTS_BOARD_BL2=stm32mp2/m33-td/mcuboot/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-bl2.dts
-DDTS_BOARD_S=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-sdcard-s.dts
-DDTS_BOARD_NS=stm32mp2/m33-td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-ns.dts
```

# CM0PLUS Demo Application (with low power workaround)

## Application Description

The Cortex-M0+ demonstration showcases the ability of the Cortex-A35 to be woken up from low power mode by the Cortex-M0+. This demonstration highlights the interprocessor communication and power management capabilities of the STM32MP2 platform.

### Key Features
- **LPUART1 and GPIOZ 7 & 8**: Prints messages on the console.
- **IPCC2**: Sends/receives notifications between Cortex-M0+ and Cortex-A35.
- **Shared Memory**: Shares data between the processors.
- **LPTIM4**: programs delay for Low power entry and exit workaround.
- **GPIOZ 9**: connected to PWR_ON for Low power exit workaround.

### Purpose
- Initialize LPUART1 and IPCC2.
- Print messages on LPUART1 every 2 seconds.
- Wait for IPCC2 interrupts indicating messages from Cortex-A35.
- Read delay from shared memory and start a timer.
- Notify Cortex-A35 after the delay to wake it up.
- Continue printing messages and waiting for new messages.

For detailed instructions and additional information, please refer to the wiki.

**This demonstration also introduces a software workaround for the Cortex-M0+ crashing when entering or leaving low power modes LPLV-Stop1/2 and Standby1 (see the STM32MP23/STM32MP25 Errata sheet).** The idea behind this workaround is to put the Cortex-M0+ in a Wait For Interrupt (WFI) state for a short time around the clock transitions induced by the low power modes. Here is a sequence diagram that provides a simplified view of this workaround implementation.

```
          ,----------.         ,-----.            ,----------.          ,-----.
          |Cortex-A35|         |EXTI2|            |Cortex-M0+|          |LPTIM|
          `----+-----'         `--+--'            `----+-----'          `--+--'
              |X|                 |                   |X|                  |
Go to low pwr |X|                 |                   |X|                  |
 -----------> |X|                 |                   |X|                  |
              |X|      SWIER      |                   |X|                  |
              |X|---------------> |                   |X|                  |
              |X|                 |      IRQ 25       |X|                  |
              |X|                 |-----------------> |X|                  |
              |X|                 |                   |X|   Start counter  |
              |X|                 |                   |X|----------------> |
              |X|                 |                   |X|                  |
              |X|                 |                   |X|---.              |
              |X|                 |                   `-'   | WFI          |
              |X|                 |                    | <--'              |
              |X|---.             |                    |                   |
              `-'   | WFI         |                    |                   |
               | <--'             |                    |                   |
               |                  |                    |                   |
           ,---------------------------------------------------!.          |
           |  Clock transition: Cortex-M0+ is in WFI state     |_\         |
           `-----------------------------------------------------'         |
               |                  |                    |                   |
               |                  |                   ,-.  IRQ 19 from TIM |
               |                  |                   |X| <----------------|
               |                  |                   |X|                  |
           ,---------------------------------------------------!.          |
           |  Cortex-A35 in low power and M0+ normal execution |_\         |
           `--------------------------------------------------------'      |
               |                  |                   |X|                  |
           PWR_ON rises           |                   |X|                  |
 -------------------------------> |                   |X|                  |
               |                  |       IRQ 6       |X|                  |
               |                  |-----------------> |X|                  |
               |                  |                   |X|   Start counter  |
               |                  |                   |X|----------------> |
               |                  |                   |X|                  |
               |                  |                   |X|---.              |
               |                  |                   `-'   | WFI          |
               |                  |                    | <--'              |
               |                  |                    |                   |
           ,---------------------------------------------------!.          |
           |  Clock transition: Cortex-M0+ is in WFI state     |_\         |
           `-----------------------------------------------------'         |
               |                  |                    |                   |
 Wake up IRQ  ,-.                 |                    |                   |
 -----------> |X|                 |                    |                   |
              |X|                 |                    |                   |
              |X|                 |                   ,-.  IRQ 19 from TIM |
              |X|                 |                   |X| <----------------|
              |X|                 |                   |X|                  |
          ,----+-----.         ,--+--.            ,----+-----.          ,--+--.
          |Cortex-A35|         |EXTI2|            |Cortex-M0+|          |LPTIM|
          `----------'         `-----'            `----------'          `-----'
```

The actual implementation adds a delay (using the LPTIM) between the PWR_ON rise and the WFI to gain execution time.

## Hardware and Software Environment to Enable the Low Power Workaround

### Hardware
- Link PWR_ON to the GPIO PZ9 (for example, on the STM32MP257f-EV1, link resistor R61 to mkB.INT).

### Software
- The main CPU must trigger an IRQ 25 in the Cortex-M0+ using the EXTI2 SWIER (line 60) *just* before entering low power.
- GPIOZ and LPTIM4 clocks must be enabled and have their autonomous mode (AM) enabled as well.
- LPTIM4 clock rate must be 32 kHz in every power mode (which means, on the STM32MP25, FLEXGEN 41 must have the LSI as reference clock with no division).

## How to Use the Low Power Workaround?
In your Cortex-M0+ firmware, include `lowpower_wa.h` and call `lowpower_wa_Init()` once before entering low power for the first time.

**Note**: The current implementation is given as an example and only works for Standby1.

## STM32CubeIDE Build Configurations
- CM0plus application with Console on LPUART1
    - **CA35TDCID_m0plus**: non-signed firmware
    - **CA35TDCID_m0plus_sign**: signed firmware

## STM32CubeIDE Debug Configuration Specifics
- **M0Plus debug configuration**
    Create debug configuration on CM0Plus application
  1. Debug configuration -> Startup -> Initialization Commands
     Ensure `mon reset` and `mon halt` are present.
     For Semihosting configurations, add `mon arm semihosting enable`.
  2. If you want to use MP2 Validation OpenOCD scripts, you shall select the `.cfg` file to use in your debug configuration.
     Ensure you have launched STM32CubeIDE with `STM32CUBEIDE_RESOURCES` environment variable pointing to `<mp2_master>\Firmware\Utilities\STM32CubeIDE_Patches\MP2_Patch\STM32CubeIDE`.
  3. Enable LPUART predefined configuration for CM0PLUS (as CM0PLUS has no access to RCC so need to configure in from OpenOCD).
     OpenOCD Options: `-bc "set mp2_preset_file MP2_PRESET_LPUART1_M0P.cfg"`

## Notes

## Keywords

## Directory Contents

## Hardware and Software Environment

- This application runs on M0Plus.
- This application has been tested with STMicroelectronics on STM32MP257F-EV1 board and can be easily tailored to any other supported devices and development boards.

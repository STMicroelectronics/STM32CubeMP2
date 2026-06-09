# FreeRTOS_ThreadCreation

CMSIS-RTOS2 / FreeRTOS thread creation example for STM32MP257F-EV1, with a CA35 secure AArch32 project and a CM33 non-secure project.

## Overview

This application demonstrates thread creation, suspend/resume sequencing, and inter-processor shutdown handling on STM32MP257F-EV1. Both core variants create the same three RTOS objects:

- one shutdown thread
- Thread One
- Thread Two

The two worker threads share a single status LED and execute a repeating 15-second pattern. The CA35 implementation additionally shows A35-specific bring-up details including UART logs, CA35 clock source selection through PLL1, HAL tick generation through TIM7, and FreeRTOS scheduler tick generation through the A35 generic timer.

## Main Capabilities

- CMSIS-RTOS2 thread creation on top of FreeRTOS
- suspend/resume coordination between two equal-priority worker threads
- LED activity pattern that makes task scheduling visible on hardware
- shutdown synchronization with the peer core through IPCC / CoproSync
- CA35 secure AArch32 bring-up with UART console and clock banner
- separate HAL time base and FreeRTOS scheduler tick on CA35

## Architecture And Key Files

| File | Role |
|------|------|
| `CA35/Secure/Core/Src/main.c` | CA35 secure AArch32 startup, UART logs, boot-mode handling, CA35 clock selection, scheduler start, and A35 tick/IRQ integration. |
| `CA35/Secure/FREERTOS/App/app_freertos.c` | CA35 shutdown thread, Thread One, Thread Two, and IPCC shutdown callback handling. |
| `CA35/Secure/FREERTOS/App/FreeRTOSConfig.h` | CA35 FreeRTOS kernel configuration, GIC settings, and scheduler tick hook selection. |
| `CA35/Secure/Core/Src/stm32mp2xx_hal_timebase_tim.c` | CA35 HAL time base implementation using TIM7. |
| `CM33/NonSecure/Core/Src/main.c` | CM33 non-secure startup, cache setup, boot-mode handling, LED init, IPCC sync, and scheduler start. |
| `CM33/NonSecure/FREERTOS/App/app_freertos.c` | CM33 shutdown thread plus the same two worker-thread scheduling pattern. |

## Runtime Flow

### CA35 Secure AArch32 (`CA35/Secure/Core/Src/main.c`)

```text
main()
  -> if DEBUG: wait in while (debug == 1)
  -> HAL_Init
  -> UART_Config
     -> print startup banner
  -> SystemClock_Config
       -> configure PLL4..PLL8 and interconnect clocks
       -> MX_CA35SS_Clock_Init
            -> configure PLL1 for CA35
            -> switch CA35 clock mux to PLL1 when needed
            -> print CA35 frequency and active source
  -> if !IS_DEVELOPER_BOOT_MODE()
       -> SystemCoreClockUpdate
       -> MX_IPCC_Init
       -> CoproSync_Init
       -> MAILBOX_SCMI_Init
     else
       -> BSP_PMIC_Init
       -> BSP_PMIC_Power_Mode_Init
  -> BSP_LED_Init(LED_GREEN)
  -> BSP_LED_Init(LED_BLUE)
  -> osKernelInitialize
  -> MX_FREERTOS_Init
  -> print "Starting FreeRTOS scheduler"
  -> osKernelStart
```

### CM33 Non-Secure (`CM33/NonSecure/Core/Src/main.c`)

```text
main()
  -> if DEBUG: wait in while (debug)
  -> HAL_Init
  -> if ICACHE_DCACHE_USE
       -> MPU_Config
       -> MX_DCACHE_Init
       -> MX_ICACHE_Init
  -> if IS_DEVELOPER_BOOT_MODE()
       -> SystemClock_Config
  -> BSP_LED_Init(LED3)
  -> if !IS_DEVELOPER_BOOT_MODE()
       -> MX_IPCC_Init
       -> CoproSync_Init
  -> osKernelInitialize
  -> MX_FREERTOS_Init
  -> osKernelStart
```

## Thread Behavior

Implemented in:

- `CA35/Secure/FREERTOS/App/app_freertos.c`
- `CM33/NonSecure/FREERTOS/App/app_freertos.c`

Common scheduling behavior:

- `Thread One`: normal priority, 512-byte stack
- `Thread Two`: normal priority, 512-byte stack
- `ShutDown Thread`: above-normal priority, 2 KB stack

15-second repeating sequence:

1. First 5 seconds:
   - Thread One toggles the demo LED every 200 ms
   - Thread Two toggles the same LED every 500 ms
2. Next 5 seconds:
   - Thread One suspends itself
   - Thread Two continues toggling every 500 ms
3. Last 5 seconds:
   - Thread Two resumes Thread One
   - Thread Two suspends itself
   - Thread One toggles every 500 ms

LED used by each image:

- CA35: `LED_GREEN`
- CM33: `LED3`

Shutdown handling:

- a shutdown semaphore is released from `CoproSync_ShutdownCb()`
- the shutdown thread deinitializes the local FreeRTOS resources
- the demo LED is deinitialized
- IPCC notifies the remote processor that local shutdown is complete

## Timing And Interrupt Model

### CA35

- HAL time base is provided by TIM7 in `stm32mp2xx_hal_timebase_tim.c`
- FreeRTOS scheduler tick is provided by the A35 generic timer through `vConfigureTickInterrupt()`
- `SystemA35_SYSTICK_TimerSourceConfig(A35_SYSTICK_PHYSICALTIMERSOURCE)` selects the timer source
- `SystemA35_SYSTICK_Config(configMAX_API_CALL_INTERRUPT_PRIORITY)` programs the scheduler tick priority
- `vApplicationFPUSafeIRQHandler()` dispatches active GIC interrupts into the CMSIS interrupt dispatcher

This split keeps HAL delays independent from the FreeRTOS scheduler tick.

### CM33

- the CM33 project uses the standard Cube / HAL interrupt flow together with the FreeRTOS CMSIS-RTOS2 integration
- `HAL_TIM_PeriodElapsedCallback()` increments the HAL tick when TIM6 expires

## LED And Error Behavior

- CA35 worker threads toggle `LED_GREEN`
- CA35 error handler blinks `LED_BLUE` continuously
- CM33 worker threads toggle `LED3`
- CM33 error handler forces `LED3` ON continuously

## Building And Debugging

### CA35 Secure

Project path:

- `STM32CubeIDE/CA35/CA35_0/Secure`

Build configurations present:

- `Debug`
- `CM33TDCID_a35_aarch32_s_sign`

Notes:

- the current CA35 application is intended for AArch32 operation
- in `Debug`, execution stops at the `debug` wait loop until the variable is changed from the debugger

### CM33 Non-Secure

Project path:

- `STM32CubeIDE/CM33/NonSecure`

Build configurations present:

- `Debug`
- `CA35TDCID_m33_ns_sign`

Notes:

- both CM33 configurations run `Utilities/optee_os/scripts/sign_rproc_fw.py`
- both CM33 configurations generate `*_sign.bin` with boot address `0x80100000`
- no launch file is checked in under the CM33 project folder in this workspace snapshot

## Boot Mode Considerations

Behavior differs between developer boot and non-developer boot:

- developer boot performs local clock and power initialization directly on the running core
- non-developer boot enables inter-processor synchronization through IPCC / CoproSync
- CA35 non-developer boot additionally initializes the SCMI mailbox

Make sure the selected CA35 / CM33 image, signing flow, and platform boot policy are aligned.

## Boot Modes (Board Straps)

| Flavor | Description | Boot0 | Boot1 | Boot2 | Boot3 | A35 Storage | M33 Storage |
|--------|-------------|-------|-------|-------|-------|-------------|-------------|
| Development | Boot Mode | 0 | 0 | 1 | 1 | DDR | DDR |
| M33-TD | Serial NOR | 1 | 1 | 0 | 1 | serial NOR | serial NOR |
| M33-TD | microSD | 1 | 1 | 1 | 0 | microSD | microSD |
| M33-TD | eMMC | 0 | 0 | 0 | 1 | eMMC | eMMC |

## Quick Start

1. Open the project in STM32CubeIDE.
2. Build the CA35 image with either `Debug` or `CM33TDCID_a35_aarch32_s_sign`, depending on your target flow.
3. Build the CM33 image with either `Debug` or `CA35TDCID_m33_ns_sign`, depending on your target flow.
4. Program or deploy the generated binaries according to your platform boot flow.
5. Run the target and observe the demo LED pattern over a full 15-second cycle.
6. For CA35 debug sessions, release the initial `debug` wait loop from the debugger before expecting LED activity or scheduler start.

## Troubleshooting

### No LED activity

- verify that the expected image is running on the intended core
- confirm that the scheduler has started and execution is not still stopped in the debug wait loop
- on CA35, verify that `LED_GREEN` initialization succeeds and the board power / resource ownership policy allows LED access

### CA35 scheduler tick not running

- check that the generic timer source is configured in `vConfigureTickInterrupt()`
- verify that the scheduler tick priority matches the FreeRTOS / GIC expectations
- confirm that the IRQ dispatch path reaches `vApplicationFPUSafeIRQHandler()` and `SystemA35_DispatchActiveInterrupt()`

### HAL delays behave incorrectly

- verify that the HAL time base remains at 1 ms
- on CA35, confirm TIM7 initialization and interrupt enable in `HAL_InitTick()`

### Shutdown handshake does not complete

- verify that IPCC is initialized on the active boot path
- check that `CoproSync_ShutdownCb()` releases the shutdown semaphore
- confirm that the local core reaches `HAL_IPCC_NotifyCPU()` after resource deinitialization

## Notes

- This example is focused on RTOS task creation and scheduling visibility rather than application payload functionality.
- The CA35 implementation is the richer bring-up reference because it includes UART diagnostics, clock selection, and explicit A35 interrupt integration.
- The CM33 implementation mirrors the thread behavior with a lighter startup sequence suited to the non-secure remote-core role.

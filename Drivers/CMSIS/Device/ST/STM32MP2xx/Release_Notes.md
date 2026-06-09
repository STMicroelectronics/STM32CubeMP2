---
pagetitle: Release Notes for STM32MP2xxx CMSIS
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# Release Notes for <mark>\ STM32MP2xxx CMSIS </mark>
Copyright &copy; 2024\ STMicroelectronics\

[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>

# Purpose

This driver provides the CMSIS device for the STM32MP2xxx products. This covers

- STM32MP257xxx ,STM32MP215xxx, STM32MP235xxx

This driver is composed of the descriptions of the registers under “Include” directory.

Various template file are provided to easily build an application. They can be adapted to fit applications requirements.

- Templates/system_stm32mp2xx_m33.c contains the initialization code referred as SystemInit.
- Startup and linker files are provided as example for STM32CubeIDE©.

:::

::: {.col-sm-12 .col-lg-8}

# __Update History__

::: {.collapse}
<input type="checkbox" id="collapse-section4" checked aria-hidden="true">
<label for="collapse-section4" checked aria-hidden="true">__V1.3.1 / 20-May-2026__</label>
<div>

## Main Changes


## Contents

- Fix Malformed comment and missing Open parentheis in some definitions
- Update ETH CMSIS definitions
- General updates


## Known Limitations

None

## Dependencies

None

</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section3" checked aria-hidden="true">
<label for="collapse-section3" checked aria-hidden="true">__V1.3.0 / 02-Feb-2026__</label>
<div>

## Main Changes

This release is the **Maintenance DV of Bit and registers definition** for STM32MP2xxx

## Contents

- Add RESERVED_9_IRQHandler symbol and wire it into the vector table at reserved IRQ 9
- A35_STARTUP_IN_ARM_MODE handled in system_stm32mp2xx_a35.c
- Align with updated CPN sheet for MP23 & MP25 for PKA IP
- MP21_A35_Baremetal : Linker script taken from PWR A35 project and added on standard path.
- Added adc macros for ADC_COMMON register set in MP21 A35 header files
- SAES Instance as per part number
- refactor(MP21): include statement for stdbool.h file added
- fix(M21): [215941]DGMCU clock enable for MP21
- MP2x: a35 startup modified to support both aarch64 and aarch32
- a35 startup modified to support both aarch64 and aarch32
- feat(MP21): USBH CMSIS added
- fix(MP2x): making ca35 system file independent buildable for LL drivers
- fix(MP2x): Fixed Illegal instruction in __set_CNTFRQ()
- fix(MP2XX): Replace include of stm32mp2xx_hal_conf.h by stm32mp2xx_hal.h in stm32mp2xx.h


## Known Limitations

None

## Dependencies

None

</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">__V1.2.0 / 30-May-2025__</label>
<div>

## Main Changes

This release is the **Maintenance DV of Bit and registers definition** for STM32MP2xxx

## Contents
- Added support of STM32MP21xx

## Known Limitations

None

## Dependencies

None

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__V1.1.0 / 23-Nov-2024__</label>
<div>

## Main Changes

This is a **v1.1.0 Release** for STM32MP25xx CMSIS

## Contents

## Known Limitations

None

## Dependencies

None

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section0" aria-hidden="true">
<label for="collapse-section0" aria-hidden="true">__V1.0.0 / 13-June-2024__</label>
<div>

## Main Changes

This is a **Mass Market release** for STM32MP2xxx CMSIS

## Contents

## Known Limitations

None

## Dependencies

None

</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on STM32 Microcontrollers </mark> ,
visit: [[www.st.com/stm32](http://www.st.com/stm32)]{style="font-color: blue;"}
</footer>

---
pagetitle: Release Notes for MFXSTM32L152 Component Drivers
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---
::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# Release Notes for <mark>MFXSTM32L152</mark> Component Driver
Copyright &copy; 2018-2021 STMicroelectronics\

[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>

# Purpose

This driver provides a set of functions needed to drive MFXSTM32L152,  IO Expander component

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section10" checked aria-hidden="true">
<label for="collapse-section10" aria-hidden="true">__V4.0.0 / 07-September-2021__</label>
<div>			

## Main Changes

-	Align TS API with BSP Common drivers V6.0.0 and above:
	-	MFXSTM32L152_TS_Start, MFXSTM32L152_TS_DetectTouch, MFXSTM32L152_TS_GetXY are no longer user APIs
	-	Add MFXSTM32L152_TS_Init, MFXSTM32L152_TS_GetState, MFXSTM32L152_TS_GetMultiTouchState, MFXSTM32L152_TS_GetGesture and MFXSTM32L152_TS_GetCapabilities
	-	Rename MFXSTM32L152_Reset into MFXSTM32L152_DeInit

## Backward Compatibility

This release breaks compatibility with previous versions.

## Dependencies

STM32Cube BSP Common drivers V6.0.0 and above

##
</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section9" aria-hidden="true">
<label for="collapse-section9" aria-hidden="true">__V3.0.3 / 31-December-2020__</label>
<div>			

## Main Changes

-	Check Misra-C 2012 coding rules compliancy
-   Formatting and miss-spelling update

##
</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section8" aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">__V3.0.2 / 30-October-2019__</label>
<div>			

## Main Changes

-	Update st_logo.png inclusion path in Release notes.

##
</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section7" aria-hidden="true">
<label for="collapse-section7" aria-hidden="true">V3.0.1 / 18-June-2019</label>
<div>			
## Main Changes
Update MFXSTM32L152_DeInit to avoid disabling IO bus that could be used by others components

##
</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section6" aria-hidden="true">
<label for="collapse-section6" aria-hidden="true">V3.0.0 / 12-April-2019</label>
<div>			
## Main Changes

### Component release

Official release of component drivers for MFXSTM32L152 in line with STM32Cube BSP drivers development guidelines (UM2298)

## Backward Compatibility

This release breaks compatibility with previous versions.

## Dependencies

STM32Cube BSP Common drivers V6.0.0

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section5" aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">V2.0.1 / 02-June-2017</label>
<div>			
## Main Changes
Update comments to be used for PDSC generation

##
</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">V2.0.0 / 24-June-2015</label>
<div>			
## Main Changes
Add Shunt management of MFXSTM32L152 component
new mfxstm32l152_IDD_ConfigShuntNbLimit() and mfxstm32l152_IDD_GetShuntUsed() APIs
Add mfxstm32l152_WriteReg() API
Note: This release must be used with BSP Common driver V4.0.0 or later

##
</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">V1.1.0 / 28-April-2015</label>
<div>			
## Main Changes
mfxstm32l152_IO_Config(): remove unnecessary delay
mfxstm32l152_TS_DetectTouch(): improve TouchScreen speed
mfxstm32l152_IDD_Config(): add configuration of number of measure to be performed, with delay between 2 measures
Note: This release must be used with BSP Common driver V3.0.0

##
</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V1.1.0 / 10-February-2015</label>
<div>			
## Main Changes
Low Power management of MFXSTM32L152 component:
New mfxstm32l152_DeInit() and mfxstm32l152_WakeUp() API
mfxstm32l152_LowPower() API completed to be MFXSTM32L152 in Standby mode
Note: This release must be used with BSP Common driver V2.2.0 or later

##
</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">V1.0.0 / 5-February-2015</label>
<div>			
## Main Changes
First official release of MFXSTM32L152 IO Expander component driver.
Note: This release must be used with BSP Common driver V2.1.0 or later.

##
</div>
:::


:::
:::

<footer class="sticky">
For complete documentation on <mark>STM32 Microcontrollers</mark> ,
visit: [[www.st.com](http://www.st.com/STM32)]{style="font-color: blue;"}
</footer>


# STM32CubeMP2 HAL Driver MPU Component

## Overview

**STM32Cube** is an STMicroelectronics original initiative to ease the developers life by reducing efforts, time and cost.

**STM32Cube** covers the overall STM32 products portfolio. It includes a comprehensive embedded software platform, delivered for each STM32 series.
   * The CMSIS modules (core and device) corresponding to the ARM(tm) core implemented in this STM32 product
   * The STM32 HAL-LL drivers : an abstraction drivers layer, the API ensuring maximized portability across the STM32 portfolio
   * The BSP Drivers of each evaluation or demonstration board provided by this STM32 series
   * A consistent set of middlewares components such as RTOS, FatFS, TCP-IP, Graphic ...
   * A full set of software projects (basic examples, applications or demonstrations) for each board provided by this STM32 series

Two models of publication are proposed for the STM32Cube embedded software:
   * The monolithic **MPU Package** : all STM32Cube software modules of one STM32 series are present (Drivers, Middlewares, Projects, Utilities) in the repo (usual name **STM32Cubexx**, xx corresponding to the STM32 series)
   * The **MPU component** : progressively from November 2019, each STM32Cube software module being part of the STM32Cube MPU Package, are delivered as an individual repo, allowing the user to select and get only the required software functions.

## Description

This **stm32mp2xx_hal_driver** MPU component repo is one element of the STM32CubeMP2 MPU embedded software package, providing the **HAL-LL Drivers** part.

## License

Copyright (c) 2024 STMicroelectronics.

This software component is licensed by STMicroelectronics under BSD-3-Clause license. You may not use this software except in compliance with the License. 
You may obtain a copy of the License [here](https://opensource.org/licenses/BSD-3-Clause).

## Release note

Details about the content of this release are available in the release note [here](https://htmlpreview.github.io/?https://github.com/STMicroelectronics/stm32mp2xx_hal_driver/blob/master/Release_Notes.html).


## Compatibility information

In this table, you can find the successive versions of this HAL-LL Driver component, in line with the corresponding versions of the full MPU package:

It is **crucial** that you use a consistent set of versions for the CMSIS Core - CMSIS Device - HAL, as mentioned in this table.

HAL Driver | CMSIS Device | CMSIS Core | Was delivered in the full MPU package
-----------| -------------| ---------- | -------------------------------------
Tag v1.2.0 | Tag v1.2.0   | Tag v5.9.0 | Tag v1.2.0 (and following, if any, till next tag)


The full **STM32CubeMP2** MPU package is available [here](https://github.com/STMicroelectronics/STM32CubeMP2).

## Troubleshooting

If you have any issue with the **Software content** of this repo, you can [file an issue on Github](https://github.com/STMicroelectronics/stm32mp2xx_hal_driver/issues/new/choose).

For any other question related to the product, the tools, the environment, you can submit a topic on the [ST Community/STM32 MPUs forum](https://community.st.com/s/group/0F90X000000AXsASAW/stm32-MPUs).

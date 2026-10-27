# STM32Cube OpenAMP integration overview

The OpenAMP package folder contains the OpenAMP package as integrated in STM32Cube for STM32MP devices.

It combines:

- the upstream OpenAMP sources
- the upstream libmetal sources
- STM32-specific middleware interface templates
- optional virtual driver helpers

---

## Subfolders in the OpenAMP package

| Folder | Purpose |
| --- | --- |
| [../libmetal](../libmetal) | Hardware abstraction layer used by OpenAMP. It provides device, I/O, IRQ, cache, memory, and synchronization services. |
| [../open-amp](../open-amp) | OpenAMP middleware itself: remoteproc, virtio, `RPMsg`, services, and public headers. |
| [app_if](app_if) | Application-facing OpenAMP templates, typically copied as `openamp_conf.h`, `openamp.h`, and `openamp.c`. |
| [platform_if](platform_if) | Platform-dependent mailbox and resource-table templates, typically copied as `mbox_ipcc.*`, `mbox_hsem.*`, and `rsc_table.*`. |
| [../virtual_driver](../virtual_driver) | Optional virtual peripheral helpers such as virtual UART, I2C, and interrupt controller support. |

### What each folder is used for in practice

- [../open-amp](../open-amp) and [../libmetal](../libmetal) provide the generic middleware code.
- [app_if](app_if) and [platform_if](platform_if) provide the STM32-specific adaptation layer copied into the user application.
- [../virtual_driver](../virtual_driver) is only needed when the application enables one of the optional virtual driver features.

### `mw_if` folder content

The `mw_if` folder is split into two parts:

- `app_if/`: application-facing OpenAMP glue
- `platform_if/`: platform-dependent mailbox and resource-table glue

The files are templates. They are typically copied into the application project,
renamed, and then customized.

#### `app_if/`

| File | Purpose | Typical copied name |
| --- | --- | --- |
| `openamp_conf_template.h` | Main OpenAMP configuration file | `openamp_conf.h` |
| `openamp_template.h` | Public OpenAMP application API | `openamp.h` |
| `openamp_template.c` | OpenAMP initialization and `RPMsg` glue | `openamp.c` |

#### `platform_if/`

| File | Purpose | Typical copied name |
| --- | --- | --- |
| `mbox_ipcc_template.h` | IPCC mailbox API | `mbox_ipcc.h` |
| `mbox_ipcc_template_mp1.c` | IPCC mailbox implementation for STM32MP1 | `mbox_ipcc.c` |
| `mbox_ipcc_template_mp2.c` | IPCC mailbox implementation for STM32MP2 | `mbox_ipcc.c` |
| `mbox_hsem_template.h` | HSEM mailbox API | `mbox_hsem.h` |
| `mbox_hsem_template.c` | HSEM mailbox implementation | `mbox_hsem.c` |
| `rsc_table_template.h` | Resource table declarations | `rsc_table.h` |
| `rsc_table_template.c` | Resource table definition and placement | `rsc_table.c` |

---

## How the pieces fit together

In an STM32Cube project, the integration is usually split into two parts:

1. **Keep upstream middleware in place**
   - use the sources from [../open-amp](../open-amp)
   - use the sources from [../libmetal](../libmetal)
   - use the sources from [../virtual_driver](../virtual_driver) when needed

2. **Copy the STM32 integration templates into the application project**
   - copy selected files from this `mw_if` folder
   - rename them to normal project files such as `openamp.c`, `openamp.h`,
     `openamp_conf.h`, `rsc_table.c`, `rsc_table.h`, and `mbox_ipcc.c`

### Template selection rules

Always copy from `app_if/`:

- `openamp_conf_template.h` -> `openamp_conf.h`
- `openamp_template.h` -> `openamp.h`
- `openamp_template.c` -> `openamp.c`

Always copy from `platform_if/`:

- `rsc_table_template.h` -> `rsc_table.h`
- `rsc_table_template.c` -> `rsc_table.c`

Then select one mailbox backend.

IPCC backend:

- `mbox_ipcc_template.h` -> `mbox_ipcc.h`
- `mbox_ipcc_template_mp1.c` -> `mbox_ipcc.c` for STM32MP1
- `mbox_ipcc_template_mp2.c` -> `mbox_ipcc.c` for STM32MP2

HSEM backend:

- `mbox_hsem_template.h` -> `mbox_hsem.h`
- `mbox_hsem_template.c` -> `mbox_hsem.c`


---

## Typical integration in an STM32Cube project

### 1. Integration in an STM32CubeIDE project

The project must compile against:

- [../open-amp](../open-amp) public headers and required source folders
- [../libmetal](../libmetal) headers and required source folders
- copied files from this `mw_if` folder, renamed to normal project files such as
  `openamp.c`, `openamp.h`, `openamp_conf.h`, `rsc_table.c`, `rsc_table.h`, and
  `mbox_ipcc.c`

Typical include paths are:

- `Middlewares/Third_Party/OpenAMP/open-amp/lib/include`
- `Middlewares/Third_Party/OpenAMP/libmetal/lib`
- `libmetal/lib/include` when the libmetal CMake pre-build step is executed inside the IDE project
- the local application folders where the copied `mw_if` files are stored

**Important constraint: `libmetal` source-tree headers are CMake templates**

- files such as `libmetal/lib/config.h`, `libmetal/lib/sys.h`, `libmetal/lib/cpu.h`,
  `libmetal/lib/atomic.h`, and several `libmetal/lib/system/...` headers contain
  placeholders such as `@PROJECT_SYSTEM@`, `@PROJECT_MACHINE@`, and `@PROJECT_PROCESSOR@`
- these headers must come from the **generated libmetal include directory**, not
  directly from the raw source tree alone
- a pre-build step for `libmetal` using CMake is mandatory

#### STM32CubeIDE linked-resource projects

In STM32CubeIDE, it is common to expose the middleware as linked resources in the
`.project` file, for example under virtual folders such as:

- `Middlewares/OpenAMP/libmetal/...`
- `Middlewares/OpenAMP/openamp/...`
- `Middlewares/OpenAMP/virtual_drivers/...`

#### OpenAMP core files

For OpenAMP, verify that the linked project includes the companion sources
required by the middleware:

| Component | Required sources |
| --- | --- |
| Remoteproc | `open-amp/lib/remoteproc/remoteproc_virtio.c` |
| RPMsg | `open-amp/lib/rpmsg/rpmsg.c`, `open-amp/lib/rpmsg/rpmsg_virtio.c` |
| Virtio | `open-amp/lib/virtio/virtio.c`, `open-amp/lib/virtio/virtqueue.c` |
| Utils | `open-amp/lib/utils/utilities.c` |

For libmetal, separate the integration into **core files** and **system-port files**.

#### libmetal core files

These files are common to both generic and FreeRTOS integrations:

| Category | Required sources |
| --- | --- |
| Core | `libmetal/lib/device.c`, `libmetal/lib/init.c`, `libmetal/lib/io.c`, `libmetal/lib/log.c`, `libmetal/lib/shmem.c` |

#### libmetal system-port files generated during CMake pre-build step

Use the system-port files that match the selected environment in the CubeIDE project.

| Environment | Files typically linked in `.project` |
| --- | --- |
| Generic / bare metal | `libmetal/lib/system/generic/condition.c`, `libmetal/lib/system/generic/device.c`, `libmetal/lib/system/generic/init.c`, `libmetal/lib/system/generic/io.c`, `libmetal/lib/system/generic/irq.c`, `libmetal/lib/system/generic/shmem.c`, `libmetal/lib/system/generic/time.c`, `libmetal/lib/system/generic/template/sys.c` |
| FreeRTOS | `libmetal/lib/system/freertos/condition.c`, `libmetal/lib/system/freertos/freertos_device.c`, `libmetal/lib/system/freertos/freertos_init.c`, `libmetal/lib/system/freertos/freertos_io.c`, `libmetal/lib/system/freertos/freertos_shmem.c`, `libmetal/lib/system/freertos/time.c`, `libmetal/lib/system/freertos/template/sys.c` |

### 2. Integration in CMake-based projects

An alternative to an STM32CubeIDE `.project`-based integration is to use a CMake
project.

The main differences are:

- in a CubeIDE project, the `.project` file lists the middleware source files to build
- in a CMake project, the file list is described in `CMakeLists.txt` and helper
  `.cmake` files
- in a CubeIDE project, `open-amp` and `libmetal` sources are often compiled
  directly inside the application project
- in a CMake project, `libmetal` is usually built first as a static library, then
  `open-amp` is built as a static library, and both libraries are linked with the
  final firmware target

For a Cortex-M33 FreeRTOS firmware, a typical CMake integration is split into a
helper middleware file and the main application `CMakeLists.txt`.

#### 2.1 Example `openamp_freertos.cmake`

The following simplified example shows the typical structure of a helper CMake file
used to build `libmetal` and `open-amp` for a Cortex-M33 FreeRTOS project:

```cmake
set(LIBMETAL_BUILD_DIR_DEFAULT "${CMAKE_BINARY_DIR}/libmetal_build/config_default")
set(LIBMETAL_BUILD_DIR "${LIBMETAL_BUILD_DIR_DEFAULT}" CACHE PATH
  "Build path of the Libmetal library")

set(OPENAMP_BUILD_DIR_DEFAULT "${CMAKE_BINARY_DIR}/openamp_build/config_default")
set(OPENAMP_BUILD_DIR "${OPENAMP_BUILD_DIR_DEFAULT}" CACHE PATH
  "Build path of the open-amp library")

set(OPENAMP_DEFINITIONS
  METAL_MAX_DEVICE_REGIONS=2
  WITH_VIRTIO_DEVICE=ON
  WITH_VIRTIO_DRIVER=OFF
  WITH_LIBMETAL_FIND=OFF
  WITH_PROXY=OFF
  WITH_DOC=OFF
  NO_ATOMIC_64_SUPPORT
  OPENAMP_ROLE=RPMSG_REMOTE
)

string(TOLOWER "FreeRTOS" PROJECT_SYSTEM)
string(TOUPPER "FreeRTOS" PROJECT_SYSTEM_UPPER)
set(MACHINE "template" CACHE STRING "")
set(WITH_STATIC_LIB ON CACHE BOOL "" FORCE)
set(WITH_DOC OFF CACHE BOOL "" FORCE)

add_subdirectory(${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal
         ${LIBMETAL_BUILD_DIR})
target_compile_definitions(metal-static PRIVATE ${OPENAMP_DEFINITIONS})

add_subdirectory(${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp
         ${OPENAMP_BUILD_DIR})
target_include_directories(open_amp-static PRIVATE
  ${LIBMETAL_BUILD_DIR}/lib/include
  ${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/include
)

add_dependencies(open_amp-static metal-static)
```

Key points:

- the helper file defines dedicated build directories for `libmetal` and
  `open-amp`
- it centralizes middleware configuration definitions such as
  `METAL_MAX_DEVICE_REGIONS=2`, `WITH_VIRTIO_DEVICE=ON`,
  `WITH_VIRTIO_DRIVER=OFF`, `WITH_LIBMETAL_FIND=OFF`, `WITH_PROXY=OFF`,
  `NO_ATOMIC_64_SUPPORT`, and `OPENAMP_ROLE=RPMSG_REMOTE`
- `libmetal` is added first with `add_subdirectory(...)`
- `open-amp` is added second and depends on `metal-static`
- the generated `libmetal` include folder is exported through
  `${LIBMETAL_BUILD_DIR}/lib/include`
- the middleware is built as `metal-static`, then `open_amp-static`, and then
  linked into the final firmware target


#### 2.2 Example application `CMakeLists.txt`

The following simplified example shows how the application target typically consumes
the middleware libraries and the copied `mw_if` files:

```cmake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "util/arm-gcc-toolchain.cmake")
set(BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/")
project(StarterApp_M33_FreeRTOS C ASM)

include(${BASE_DIR}/cmake/openamp_freertos.cmake)

set(APPLICATION_SRC
  ${BASE_DIR}/CM33/NonSecure/Core/Src/main.c
  ${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/mbox_ipcc.c
  ${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/openamp.c
  ${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/rsc_table.c
)

add_executable(${PROJECT_NAME}.elf ${APPLICATION_SRC})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
  ${BASE_DIR}/CM33/NonSecure/OpenAMP/Inc
  ${LIBMETAL_BUILD_DIR}/lib/include
  ${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/include
)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE
  metal-static
  open_amp-static
)
```

Key points:

- the application `CMakeLists.txt` declares the firmware target and includes the
  OpenAMP helper CMake file
- the copied `mw_if` sources remain in the application target
- the application includes its local `OpenAMP/Inc` folder for copied headers
- the application links with `metal-static` and `open_amp-static`
- generated middleware include directories are added explicitly
- unlike the STM32CubeIDE linked-resource flow, the middleware source list is
  described in CMake files instead of in IDE project metadata

### 3. Copy the STM32 integration templates

From [mw_if](mw_if), copy the files needed by the target application.

For a typical STM32MP2 + IPCC project:

- `app_if/openamp_conf_template.h` -> `openamp_conf.h`
- `app_if/openamp_template.h` -> `openamp.h`
- `app_if/openamp_template.c` -> `openamp.c`
- `platform_if/rsc_table_template.h` -> `rsc_table.h`
- `platform_if/rsc_table_template.c` -> `rsc_table.c`
- `platform_if/mbox_ipcc_template.h` -> `mbox_ipcc.h`
- `platform_if/mbox_ipcc_template_mp2.c` -> `mbox_ipcc.c`

#### What to customize in the copied files

This section focuses on the main `openamp_conf.h` macros that are usually adapted
when the template files are copied into a project.

| Macro | Cortex-M Baremetal | Cortex-M FreeRTOS | Cortex-A Baremetal | Cortex-A FreeRTOS | Comment |
| --- | --- | --- | --- | --- | --- |
| `OPENAMP_USE_FREERTOS` | `not defined` | `defined` | `not defined` | `defined` | Enable only for FreeRTOS-based projects. |
| `OPENAMP_USE_TFM_NS_API` | `optional` | `optional` | `not defined` | `not defined` | Enable when TF-M manages the Cortex-M non-secure context and provides the resource-table information through the TF-M platform APIs. |
| `LINUX_RPROC_MASTER` | `optional` | `optional` | `optional` | `optional` | Legacy compatibility define used to retrieve the resource table from the ELF file. Set it for STM32MP15 projects and for STM32MP2x projects using the OSTL-A35TD flavor. |
| `MAILBOX_IPCC_IF_ENABLED` | `defined*` | `defined*` | `defined*` | `defined*` | Preferred mailbox backend on STM32MP projects using IPCC. |
| `MAILBOX_HSEM_IF_ENABLED` | `not defined*` | `not defined*` | `not defined*` | `not defined*` | Use instead of IPCC only when the project explicitly relies on HSEM. |
| `RSC_TABLE_USE_BCKUP_REG` | `optional` | `optional` | `optional` | `optional` | Enable when the resource-table address and length are exchanged through backup registers instead of a fixed linked resource-table memory area. |
| `VRING_RX_ADDRESS` | `-1` | `-1` | project-specific or template default | project-specific or template default | The value is fixed by the `RPMSG_HOST` role in the resource table, depending on the memory map. |
| `VRING_TX_ADDRESS` | `-1` | `-1` | project-specific or template default | project-specific or template default | The value is fixed by the `RPMSG_HOST` role in the resource table, depending on the memory map. |
| `VRING_BUFF_ADDRESS` | `-1` | `-1` | project-specific or template default | project-specific or template default | The value is fixed by the `RPMSG_HOST` role in the resource table, depending on the memory map. |
| `VRING_ALIGNMENT` | `16` | `16` | `not applicable` | `not applicable` | Keep the default value unless the platform integration requires a change. |
| `VRING_NUM_BUFFS` | `16` | `16` | `not applicable` | `not applicable` | Defines the number of buffers preallocated in the vrings for `RPMsg` communication. |
| `VIRTUAL_UART_MODULE_ENABLED` | `optional` | `optional` | `optional` | `optional` | Enable only when the project uses the virtual UART helper. |
| `VIRTUAL_I2C_MODULE_ENABLED` | `optional` | `optional` | `optional` | `optional` | Enable only when the project uses the virtual I2C helper. |
| `VIRTUAL_INTC_MODULE_ENABLED` | `optional` | `optional` | `optional` | `optional` | Enable only when the project uses the virtual interrupt controller helper. |
| `__LOG_TRACE_IO_` / `__LOG_UART_IO_` | `optional` | `optional` | `optional` | `optional` | Enable only if logging support is required. |

\* Define exactly one mailbox backend: `MAILBOX_IPCC_IF_ENABLED` or
`MAILBOX_HSEM_IF_ENABLED`.

The other copied files are then adapted around these configuration choices:

- `openamp.c` and `openamp.h` implement the initialization, `RPMsg` setup,
  endpoint creation, and polling flow
- `rsc_table.c` and `rsc_table.h` must match the chosen memory map and boot flow
- `mbox_ipcc.c` or `mbox_hsem.c` must match the selected mailbox backend

Additional notes for optional macros:

- `OPENAMP_USE_TFM_NS_API` is relevant only on projects where TF-M manages the
  secure/non-secure interface for the Cortex-M side
- `LINUX_RPROC_MASTER` is a legacy preprocessor define used to retrieve the resource table from the `.resource_table` section in the ELF file. It should be enabled:
  - on STM32MP15
  - on STM32MP2x when using the OSTL-A35TD flavor
- `RSC_TABLE_USE_BCKUP_REG` is relevant only when the project boot flow relies on
  backup registers to publish the resource-table location

Important fixed values in `openamp_conf.h` should normally stay unchanged:

- `NUM_RESOURCE_ENTRIES`
- `VRING_COUNT`
- `VDEV_ID`
- `VRING0_ID`
- `VRING1_ID`
- `SHM_START_ADDRESS`
- `SHM_SIZE`

### 4. Configure the libmetal pre-build step

The STM32MP integration uses a CMake pre-build step to configure libmetal for the
selected environment.

Typical toolchain files are:

| Environment | libmetal toolchain file |
| --- | --- |
| Bare metal / generic | `../libmetal/cmake/platforms/stm32mp-generic-gcc.cmake` |
| FreeRTOS | `../libmetal/cmake/platforms/stm32mp-freertos-gcc.cmake` |

Typical pre-build command pattern:

```sh
cmake -S ../../Middlewares/Third_Party/OpenAMP/libmetal \
  -B <project-build-dir>/libmetal \
  -G "Unix Makefiles" \
  -DCMAKE_TOOLCHAIN_FILE=cmake/platforms/stm32mp-generic-gcc.cmake
```

Why this step is needed:

- libmetal configures platform-specific options through CMake
- generated headers are placed in the libmetal build directory
- the IDE build must see those generated headers before compiling the application

After adding the pre-build step, also add the generated libmetal include directory
from the build output to the compiler include paths.

Typical generated libmetal include path:

- `<build-dir>/libmetal/lib/include`

In CubeIDE, when the pre-build output directory is created inside the project tree,
this folder usually appears directly as:

- `libmetal/lib/include`

This generated folder must be added to the IDE include search path, because it
contains the configured libmetal headers generated by CMake.

### 5. Add the required compile-time defines in the project

An STM32Cube project using these middleware sources usually needs defines for:

- OpenAMP role selection
- mailbox backend selection
- libmetal internal configuration
- optional RTOS or logging support

The following table gives the **typical STM32MP values** for four project flavors.

#### STM32CubeIDE linked-resource projects

Add the following preprocessor defines to the IDE project.

| Define | Cortex-M Baremetal | Cortex-M FreeRTOS | Cortex-A Baremetal | Cortex-A FreeRTOS | Comment |
| --- | --- | --- | --- | --- | --- |
| `VIRTIO_DEVICE_SUPPORT` | `1` | `1` | `0` | `0` | Cortex-M is typically the OpenAMP remote/device side. |
| `VIRTIO_DRIVER_SUPPORT` | `0` | `0` | `1` | `1` | Cortex-A is typically the OpenAMP host/driver side. |
| `NO_ATOMIC_64_SUPPORT` | `defined` | `defined` | `not defined` | `not defined` | Typically required on Cortex-M. |
| `METAL_INTERNAL` | `defined` | `defined` | `defined` | `defined` | libmetal internal API is typically enabled on STM32 projects. |
| `METAL_MAX_DEVICE_REGIONS` | `2` | `2` | `2` | `2` | `openamp.c` uses one shared-memory region and one resource-table region. |
| `VQ_RX_EMPTY_NOTIFY` | `0` | `0` | `0` | `0` | Set to `1` to notify the remote side only when all buffers have been treated. |

#### CMake-based projects

Add the following CMake configuration values to the project.

| Define | Cortex-M Baremetal | Cortex-M FreeRTOS | Cortex-A Baremetal | Cortex-A FreeRTOS | Comment |
| --- | --- | --- | --- | --- | --- |
| `WITH_VIRTIO_DEVICE` | `ON` | `ON` | `OFF` | `OFF` | Cortex-M is typically the OpenAMP remote/device side. |
| `WITH_VIRTIO_DRIVER` | `OFF` | `OFF` | `ON` | `ON` | Cortex-A is typically the OpenAMP host/driver side. |
| `NO_ATOMIC_64_SUPPORT` | `defined` | `defined` | `not defined` | `not defined` | Typically required on Cortex-M. |
| `WITH_LIBMETAL_FIND` | `OFF` | `OFF` | `OFF` | `OFF` | Build OpenAMP against the in-tree `libmetal` target instead of searching for a separately installed package. |
| `WITH_PROXY` | `OFF` | `OFF` | `OFF` | `OFF` | Disable the optional proxy component when it is not used by the application. |
| `WITH_DOC` | `OFF` | `OFF` | `OFF` | `OFF` | Disable documentation generation in embedded firmware builds. |
| `METAL_MAX_DEVICE_REGIONS` | `2` | `2` | `2` | `2` | `openamp.c` uses one shared-memory region and one resource-table region. |
| `OPENAMP_ROLE` | `RPMSG_REMOTE` | `RPMSG_REMOTE` | `RPMSG_HOST` | `RPMSG_HOST` | Select the OpenAMP role built into the middleware libraries. |

### 6. Update the linker script

The project linker file must reserve shared memory for OpenAMP and export the
symbols used by `openamp_conf.h`.

This step is mandatory because `RPMsg` vrings and buffers must match the memory map
used by the `remoteproc` / OpenAMP setup.

- Define the memory region used for the `RPMsg` / virtio rings and buffers:
  - `__OPENAMP_region_start__`
  - `__OPENAMP_region_end__`

- Define the resource table location:
  - for Linux `remoteproc`-based flows, the linker file may need a dedicated
    `.resource_table` section so the remote firmware exposes its resource table
    correctly
  - otherwise, the linker script must define the resource table memory symbols:
    - `__RSC_TABLE_region_start__`
    - `__RSC_TABLE_region_end__`

### 7. Initialize OpenAMP from the application

After the middleware files are added to the project:

1. initialize the mailbox peripheral used by the target
2. call `MX_OPENAMP_Init()`
3. call `MX_OPENAMP_RPMsg_Init()`
4. create `RPMsg` endpoints with associated callbacks for message reception
5. process traffic with `OPENAMP_check_for_message()` or the RTOS-assisted flow

The helper APIs are provided by the copied `openamp.c` and `openamp.h` files from
[app_if](app_if).

---

## Recommended project reference

A good CubeIDE-style reference project is `OpenAMP_TTY_echo`:

- keep [../open-amp](../open-amp) and [../libmetal](../libmetal) under
  `Middlewares/Third_Party/OpenAMP`
- copy templates from this folder into application-owned folders such as
  `OpenAMP/Inc` and `OpenAMP/Src`
- add the required source/include references from the IDE project settings
- add the generated include directories produced by the CMake pre-build step
- keep linker and compiler options in the application project, not in the middleware package

---

## Where to find more details

- OpenAMP open source project documentation: [openamp.readthedocs.io](https://openamp.readthedocs.io/en/latest/)
- OpenAMP upstream sources: [../open-amp/README.md](../open-amp/README.md)
- libmetal upstream sources: [../libmetal/README.md](../libmetal/README.md)

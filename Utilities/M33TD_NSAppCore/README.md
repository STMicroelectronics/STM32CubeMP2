# M33TD_NSAppCore (Common Stack)

Reusable, board-agnostic CM33-NonSecure (FreeRTOS) application stack for STM32MP2 M33TDCID projects.

In practice, projects like StarterApp / Template_StarterApp build a **Non‑Secure Application Manager** on top of this stack: the project provides board-specific drivers and configuration, then calls `NSCoreApp_Init()` as the bootstrap entry point.

This stack provides the task-level building blocks needed to support the typical STM32MP2 M33TDCID boot and runtime flow: MCUboot brings up the platform and chains to Trusted Firmware-M (TF-M) on CM33-S, then the CM33-NS application starts and uses TF-M secure services. The Cortex-A35 remains available as a high-performance coprocessor that can be started/monitored (and interacted with via RPMsg when enabled) from CM33-NS.

---

## What you get

- Portable CM33-NS tasks (required Logger, RemoteProc, SCMI Manager, Watchdog Monitor, plus optional UserApp, Button, OpenAMP, Display, Low Power Manager, and Firmware Update Manager).
- Thin driver interfaces (in `Includes/`) so each project can provide its own board/peripheral integration without forking this stack.
- Config templates (in `Config/`) to keep feature enablement and task sizing centralized.
- Cross-task orchestration for suspend/resume, RPMsg control, and structured firmware-update flows without pushing board-specific logic into the utility.
- A post-build utility (in `postbuild/`) to assemble/sign the final TF-M S/NS combined image and copy flashing-ready artifacts into a project-local `bin/` folder (created at `${BASE_DIR}/bin`).

---

## Architecture (integration flow)

```text
┌──────────┐  ┌──────────────┐  ┌──────────────────────┐
│ MCUboot  │->│ TF-M (CM33-S)│->│ CM33-NS FreeRTOS app │
└──────────┘  └──────────────┘  └──────────────────────┘
                                     |
                                     v
                  ┌──────────────────────────────┐
                  │ NSCoreApp_Init() (bootstrap) │
                  └──────────────────────────────┘
                                     |
                                     v
                  ┌──────────────────────────────┐
                  │ M33TD_NSAppCore tasks        │
                  └──────────────────────────────┘
                     |          |            |
                     |          |            +-> OpenAMP/libmetal (optional) -> RPMsg
                     |          +-> Project drivers (CM33/.../AppDriver)
                     +-> TF-M NS APIs (CPU/WDT/SCMI/notifications)
                                     |
                                     v
                              Cortex-A35 (Linux/BareMetal)
```

---

## Task-level view (simplified)

The stack is structured around a single bootstrap and a set of tasks.

```text
NS Application Manager (your project)
   |
   +-> NSCoreApp_Init()
            |
            +-> LoggerTask            (required)
            +-> ScmiMgrTask           (required)
            +-> RemoteProcTask        (required)
            +-> WdgMonitorTask        (required)
            +-> LowPowerMgrTask       (optional)
            +-> UserAppTask           (optional)
            +-> BtnMonitorTask        (optional)
            +-> OpenAMPTask           (optional)
            +-> DisplayTask           (optional)
            +-> FwuMgrTask            (optional)

Each task typically:
   - Uses TF-M NS APIs for secure services (CPU/WDT/SCMI)
   - Calls a thin driver interface in Utilities/M33TD_NSAppCore/Includes/
      implemented by the project in CM33/.../M33TD_NSAppCore/AppDriver/
```

Current initialization order in `NSCoreApp_Init()` is:

- Logger
- TF-M NS interface initialization + startup logs
- Optional LowPowerMgr, UserApp, BtnMonitor, Display
- ScmiMgr
- Optional OpenAMP
- RemoteProc
- WdgMonitor
- Optional FwuMgr

---

## Task dependencies and feature bundles

The stack supports multiple integration “profiles” by enabling/disabling tasks. Some tasks are independent, but others are feature carriers and must be enabled together to deliver an end-to-end functionality.

### Core tasks (baseline)

These are the baseline “Non‑Secure Application Manager” services and are expected to remain enabled in most projects:

- **LoggerTask**: logging infrastructure.
- **ScmiMgrTask**: SCMI notification/event handling.
- **RemoteProcTask**: CA35 start/stop/monitor + crash recovery.
- **WdgMonitorTask**: watchdog supervision.

### Inter-processor communication (RPMsg)

Inter-processor communication between Linux/A35 and CM33-NS is provided by **OpenAMPTask**.

When `ENABLE_OPENAMP_TASK=1`, OpenAMPTask initializes OpenAMP/libmetal and creates RPMsg endpoints. In the current stack, OpenAMPTask acts as the transport hub for four categories of use-cases:

1. **Display-related RPMsg events (Linux → CM33)**
   - When `ENABLE_DISPLAY_TASK=1`, OpenAMPTask registers a **Display RPMsg endpoint** and forwards inbound messages to the DisplayTask callback.
   - This is used for Linux-driven display interactions such as stopping a splash screen and starting reboot-related display flows.
   - If DisplayTask is disabled, display-endpoint code is excluded from OpenAMPTask by conditional compilation.

2. **M33-initiated power requests (CM33 → Linux/A35)**
   - OpenAMPTask exposes a **Power RPMsg endpoint** used to send simple power-management commands to the remote side (e.g., `reboot` / `shutdown`).
   - The reference wiring uses BtnMonitorTask to trigger a reboot request on a **very long press** (see `App/Src/openamp_task.c` and `App/Src/btn_monitor_task.c`).
   - OpenAMPTask can also be used without BtnMonitorTask: any project code can post an OpenAMP command to request reboot/shutdown.

3. **Low-power policy control (Linux → CM33)**
   - When `ENABLE_LOW_POWER_MGR_TASK=1`, OpenAMPTask registers a **Low Power RPMsg endpoint** and forwards policy commands to LowPowerMgrTask.
   - This lets the remote side limit or restore the current suspend policy while LowPowerMgrTask remains the owner of the actual suspend/resume sequence.

4. **Firmware update control and responses (Linux ↔ CM33)**
   - When `ENABLE_FWU_MGR_TASK=1`, OpenAMPTask registers a **FWU RPMsg endpoint** used by FwuMgrTask to receive commands and return structured status/info responses.
   - FWU support is transport-coupled to OpenAMP: `ENABLE_FWU_MGR_TASK=1` requires `ENABLE_OPENAMP_TASK=1`.

**Project-level OpenAMP binding (required when `ENABLE_OPENAMP_TASK=1`)**

OpenAMPTask depends on a project-provided OpenAMP “glue layer” (OpenAMP MW integration files) that implements the APIs used by `App/Src/openamp_task.c`.

- The ST middleware provides *templates* in `Firmware/Middlewares/Third_Party/OpenAMP/mw_if/app_if/openamp_template.c/.h` and `openamp_conf_template.h`, but they are **not used as-is** in this stack.
- For this OpenAmpTask, the template must be adapted (callbacks/transport) so that mailbox RX notifications are forwarded into the task. In particular, the `openamp.c/.h` used by your project should provide a **register API** so the OpenAMP MW can notify the stack on RX events.
- Recommended approach: copy/adapt the OpenAMP integration from the **full featured application reference** listed in [Reference projects](#reference-projects) (OpenAMP MW glue: `openamp.c/.h`, `openamp_conf.h`, plus the project’s IPCC/MPU setup in `openamp_driver.c`).

If your project does not need RPMsg transport for display, power control, low-power policy control, or firmware-update messaging, you can keep `ENABLE_OPENAMP_TASK=0`.

### Low-power orchestration

LowPowerMgrTask centralizes suspend-policy ownership and low-power sequencing.

- It is enabled with `ENABLE_LOW_POWER_MGR_TASK=1`.
- It owns the aggregate suspend policy, combining a default policy (`LOW_POWER_DEFAULT_POLICY_ENABLE`) with optional runtime agent constraints.
- It coordinates low-power entry and resume with RemoteProcTask, SCMI notifications, and project sleep hooks exposed through `Includes/low_power_mgr_driver.h`.
- When OpenAMP is enabled, it can also accept RPMsg policy commands from the remote side.

Low-power behavior spans multiple tasks by design:

- **LowPowerMgrTask** owns the policy and state machine.
- **ScmiMgrTask** forwards SCMI suspend-related notifications.
- **RemoteProcTask** performs A35 suspend/resume transitions and state tracking.
- **OpenAMPTask** optionally carries low-power RPMsg commands.
- **BtnMonitorTask** and **WdgMonitorTask** can register listeners to mask inputs or ping the watchdog around deep sleep entry.


### Button Monitor usage

BtnMonitorTask is a generic button press classifier:

- It can be used independently by registering listeners for button events.
- In the reference wiring, it is also used as an input trigger for M33-initiated reboot through OpenAMPTask.

### Display pipeline enablement

Display functionality is split into:

- **DisplayTask**: display control logic (splash, panel init, overlay/reboot-related flows, etc.).
- **OpenAMPTask display endpoint**: RPMsg plumbing used to receive display-related messages from Linux and dispatch them to DisplayTask.

This is why display-related RPMsg handling is typically enabled as a bundle:

- `ENABLE_DISPLAY_TASK=1` + `ENABLE_OPENAMP_TASK=1`

Projects commonly expose a “minimal/headless” build profile where the display pipeline is disabled. In that case, other tasks (including OpenAMPTask for power requests and BtnMonitorTask) can still be enabled if desired.

### Firmware update flow

FwuMgrTask provides a structured firmware-update control point on the CM33-NS side.

- It is enabled with `ENABLE_FWU_MGR_TASK=1`.
- It depends on OpenAMP transport and therefore requires `ENABLE_OPENAMP_TASK=1`.
- It receives FWU commands over RPMsg, queries PSA FWU component information, tracks pending updates, and coordinates install/reboot flows.
- When applying pending images, it stops the remote A35 side before writing staged images through PSA FWU APIs and requesting the reboot sequence.

This task is utility-owned logic; the project remains responsible for providing the platform integration needed by OpenAMP and the PSA/TF-M FWU environment.

For common end-to-end combinations of these tasks (profiles), see [Recommended profiles](#recommended-profiles-examples) in the **Feature toggles (generic)** section.

## Repository layout

```text
M33TD_NSAppCore/
  |-- App/           (task APIs and implementations)
  |     |-- Inc/
  |     |-- Src/
  |
  |-- Includes/      (public driver headers)
  |-- Common/        (shared helpers, optional FaultMgr)
  |     |-- FaultMgr/
  |
  |-- Config/        (templates to copy/override in project)
  |-- Assets/        (optional splash/animation assets)
  |-- postbuild/     (postbuild utilities)
```

- **App/**  
  RTOS tasks and NS application logic. Public task APIs live in `App/Inc/`, implementations in `App/Src/`.

- **Includes/**  
  Public driver interfaces that projects must implement. The project provides the concrete drivers that bind tasks to the actual board/peripherals (UART/GPIO/display/etc.), typically under `CM33/.../M33TD_NSAppCore/AppDriver/`.

- **Common/**  
  Shared helpers used by multiple tasks, plus optional FaultMgr support under `Common/FaultMgr/` and shared panel/display helpers under `Common/Panel/`.

- **Config/**  
  Configuration header templates. Projects copy and adapt these (typically into `CM33/NonSecure/FREERTOS/App/`) to control feature enablement and task sizing.

- **Assets/**  
  Optional visual assets (splash screen, animations, visual watchdog banners, OLED assets, etc.) for display-capable builds.

- **postbuild/**  
  Standalone CMake post-build utility for assembling NS + Secure binaries, signing the combined image, and copying outputs into `${BASE_DIR}/bin/`.

---

## How projects integrate this stack

### CMake-based projects (recommended)

Reference integration:  
`Firmware/Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD`

Typical wiring pattern:

1. **Compile stack sources**
   - Import sources from `Utilities/M33TD_NSAppCore/App/Src` (and optionally `Common/FaultMgr`).
   - The reference project groups these sources in `cmake/nsappcore_sources.cmake`.

2. **Add include directories**
   - `Utilities/M33TD_NSAppCore/App/Inc`
   - `Utilities/M33TD_NSAppCore/Includes`
   - `Utilities/M33TD_NSAppCore/Common` (and `Common/FaultMgr` if enabled)

3. **Provide project drivers**
   - Implement the driver interfaces declared in `Utilities/M33TD_NSAppCore/Includes/*.h`.
   - Place implementations under your project, typically:
     - `CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/`.

4. **Provide configuration headers**
   - Copy templates from `Utilities/M33TD_NSAppCore/Config/` into your project (commonly `CM33/NonSecure/FREERTOS/App/`).
   - Rename to:
     - `app_tasks_config.h`
     - `nsappcore_config.h`

5. **Call the stack bootstrap**
   - From your `main.c` (after the RTOS kernel is initialized): call `NSCoreApp_Init()`.
   - If FaultMgr is enabled, call `FAULT_Init()` before starting tasks.

### Feature toggles (generic)

The stack is primarily controlled through preprocessor macros that accept numeric values `0` (disabled) and `1` (enabled).

Projects may expose these macros as CMake cache options like `ON|OFF`, but the **code-level contract** is always `0|1`.

Defaults shown below match the default macro values in:

- `Utilities/M33TD_NSAppCore/Config/nsappcore_config_template.h` (NSAppCore integration template)
- `Utilities/M33TD_NSAppCore/Common/FaultMgr/fault_config.h` (FaultMgr defaults; can be overridden by defining macros before including it)

| Feature                 | Macro                              | Default | Values    | Notes                                                                  |
|-------------------------|------------------------------------|---------|-----------|------------------------------------------------------------------------|
| Auto-start CA35 at boot | `REMOTE_PROC_AUTO_START`          | `1`     | `0` / `1` | Used by RemoteProcTask to decide whether to request CA35 start.       |
| Default low-power policy| `LOW_POWER_DEFAULT_POLICY_ENABLE` | `0`     | `0` / `1` | When `1`, LowPowerMgrTask starts from its default suspend policy.     |
| Real-time debug log     | `REALTIME_DEBUG_LOG_ENABLED`      | `0`     | `0` / `1` | When `1`, logs print directly (printf) instead of via logger queue.   |
| Fault exception         | `FAULT_EXCEPTION_ENABLE`          | `1`     | `0` / `1` | Enables FaultMgr exception capture/reporting (if FaultMgr is linked). |
| Fault backtrace         | `FAULT_EXCEPTION_BACKTRACE_ENABLE`| `1`     | `0` / `1` | Enables backtrace capture when FaultMgr is enabled.                   |

Task-level enable/disable (for example `ENABLE_OPENAMP_TASK`, `ENABLE_LOW_POWER_MGR_TASK`, `ENABLE_FWU_MGR_TASK`, `ENABLE_BTN_MONITOR_TASK`, `ENABLE_DISPLAY_TASK`) is controlled via `app_tasks_config.h` and can be overridden by project-level defines (typically via `nsappcore_config.h` or build flags).

### Task enable defaults and override precedence

The template task defaults come from:  
`Utilities/M33TD_NSAppCore/Config/app_tasks_config_template.h`

| Task           | Enable macro              | Default                     |
|----------------|---------------------------|-----------------------------|
| LoggerTask     | `ENABLE_LOGGER_TASK`      | `1` (required)              |
| ScmiMgrTask    | `ENABLE_SCMI_MGR_TASK`    | `1` (required)              |
| RemoteProcTask | `ENABLE_REMOTEPROC_TASK`  | `1` (required)              |
| WdgMonitorTask | `ENABLE_WDG_MONITOR_TASK` | `1` (required)              |
| LowPowerMgrTask| `ENABLE_LOW_POWER_MGR_TASK` | `0`                       |
| UserAppTask    | `ENABLE_USERAPP_TASK`     | `0`                         |
| BtnMonitorTask | `ENABLE_BTN_MONITOR_TASK` | `0`                         |
| OpenAMPTask    | `ENABLE_OPENAMP_TASK`     | `0`                         |
| DisplayTask    | `ENABLE_DISPLAY_TASK`     | `0` (auto-forced to `1` if `DISPLAY_PANEL_ENABLED` is defined) |
| FwuMgrTask     | `ENABLE_FWU_MGR_TASK`     | `0` (requires `ENABLE_OPENAMP_TASK=1`) |

### Recommended profiles (examples)

These examples show common combinations of tasks for typical end-to-end behaviors. Use them as starting points and adapt them to your product needs.

In the table below, `0/1` means “optional depending on your application logic”.

| Profile | `ENABLE_OPENAMP_TASK` | `ENABLE_DISPLAY_TASK` | `ENABLE_BTN_MONITOR_TASK` | `ENABLE_LOW_POWER_MGR_TASK` | `ENABLE_FWU_MGR_TASK` | Intended use |
|---|---:|---:|---:|---:|---:|---|
| Bare Minimum (no RPMsg) | 0 | 0 | 0 | 0 | 0 | RemoteProc + SCMI + watchdog; no inter-processor messaging |
| Button only (no RPMsg) | 0 | 0 | 1 | 0 | 0 | Local button events for CM33-side logic; no Linux/A35 interaction |
| Power control (M33 → Linux) | 1 | 0 | 0/1 | 0 | 0 | M33-initiated reboot/shutdown requests via the Power RPMsg endpoint |
| Low-power control | 1 | 0 | 0/1 | 1 | 0 | Suspend policy owned by LowPowerMgrTask, optionally updated over RPMsg |
| Display control (Linux → M33) | 1 | 1 | 0/1 | 0/1 | 0 | Linux-driven display events delivered to DisplayTask via RPMsg |
| FWU-capable | 1 | 0/1 | 0/1 | 0/1 | 1 | Structured firmware-update messaging and install/reboot flows over RPMsg |
| Full featured HMI/control | 1 | 1 | 1 | 1 | 0/1 | Combined display endpoint, low-power orchestration, and button-triggered remote power requests |

**How this maps to the provided reference projects**

- Template_StarterApp_M33TD is a good bare-minimum starting point and is closest to **Bare Minimum (no RPMsg)**.
- StarterApp_M33TD is a feature-rich application reference and is closest to **Full featured HMI/control**.
- FWU and low-power capabilities are opt-in overlays on top of these baseline profiles; enable them only when the corresponding platform integration is present.

See: [Reference projects](#reference-projects).

**Why `#ifndef` matters**  
Each `ENABLE_*` macro is defined with a `#ifndef` guard in `app_tasks_config.h`. That means the project can override the default by defining the macro *before* `app_tasks_config.h` is included.

In the template `nsappcore_config_template.h`, the include order is:

```text
nsappcore_config.h
   |
   +-> (optional) higher-level build options map to ENABLE_* and feature macros
   +-> (optional) defines ENABLE_DISPLAY_TASK=1 if DISPLAY_PANEL_ENABLED is set
   +-> includes app_tasks_config.h  (defaults apply only if still undefined)
```

For correctness, the stack also enforces that the core tasks remain enabled via compile-time checks in `nsappcore_config.h`.

### How to set macros

**CMake (generic pattern)**

Set macros on the target that builds your CM33-NS application:

```cmake
target_compile_definitions(<your_target> PRIVATE
   REMOTE_PROC_AUTO_START=1
   REALTIME_DEBUG_LOG_ENABLED=0
   FAULT_EXCEPTION_ENABLE=1
   FAULT_EXCEPTION_BACKTRACE_ENABLE=0
)
```

If your build system exposes higher-level options, ensure they ultimately define these macros (as `0|1`) for the CM33-NS target.

**STM32CubeIDE**

Add macro definitions in:

`Project Properties -> C/C++ Build -> Settings -> Tool Settings -> MCU GCC Compiler -> Preprocessor`

Examples:

- `REMOTE_PROC_AUTO_START=1`
- `REALTIME_DEBUG_LOG_ENABLED=0`

### STM32CubeIDE projects

The model is the same: link stack sources and headers, but keep board-specific code local to the project.

Recommended separation:

- **Stack sources**: linked from `Utilities/M33TD_NSAppCore` (do not copy).
- **Board specifics**: implemented in the project under `CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/`.
- **Configuration**: keep `app_tasks_config.h` and `nsappcore_config.h` under:
  - `CM33/NonSecure/FREERTOS/App/`

## Task catalog (what calls what)

The stack follows a consistent pattern:

- Task implementation lives in `App/Src/*_task.c`.
- Each task calls into a driver interface in `Includes/*_driver.h`.
- Your project provides the driver implementation in `.../AppDriver/*_driver.c`.

| Task          | Purpose                                   | Enablement                                      | Main dependencies                          |
|---------------|-------------------------------------------|-------------------------------------------------|--------------------------------------------|
| NSCoreApp     | Bootstraps and starts enabled tasks       | Always                                          | FreeRTOS/CMSIS-RTOS2                       |
| LoggerTask    | Central logging + sinks                   | Always (typically)                              | Project logger output driver               |
| LowPowerMgrTask | Suspend policy owner and low-power sequencing | `ENABLE_LOW_POWER_MGR_TASK`                 | Project low-power hooks, RemoteProcTask, SCMI, optional OpenAMP transport |
| UserAppTask   | Example app logic (LED, liveness demo)    | Enabled by config                               | Project LED driver                         |
| RemoteProcTask| Start/stop/suspend/resume/monitor CA35 via TF-M | Enabled by config + `REMOTE_PROC_AUTO_START` | TF-M CPU IOCTL, project remoteproc driver, SCMI/LowPower/FWU interactions |
| ScmiMgrTask   | Handle SCMI notifications and power events | Enabled by config                              | TF-M SCMI + notification APIs, optional LowPower/OpenAMP listeners |
| WdgMonitorTask| Watchdog ping/supervision                 | Enabled by config                               | TF-M WDT IOCTL, optional LowPower listener |
| BtnMonitorTask| Button press classification               | `ENABLE_BTN_MONITOR_TASK`                       | Project button driver, optional OpenAMP and LowPower listeners |
| OpenAMPTask   | RPMsg endpoints + transport/control messages | `ENABLE_OPENAMP_TASK`                         | OpenAMP/libmetal, mailbox/IPCC platform, Display/LowPower/FWU task callbacks |
| DisplayTask   | Display control logic and UI flows        | `ENABLE_DISPLAY_TASK`                           | Project display driver, optional OpenAMP transport for Linux-driven display control |
| FwuMgrTask    | Structured firmware-update command handling | `ENABLE_FWU_MGR_TASK`                         | PSA FWU APIs, devicetree staging layout, RemoteProcTask, OpenAMPTask |

---

## Postbuild Utility: Binary Assembly and Signing

The `postbuild/` directory contains a CMake project that automates the final steps of binary assembly and signing for STM32MP2 M33TD applications. It combines the built Non‑Secure (NS) application with Trusted Firmware‑M (TF‑M) Secure binaries, producing signed images and deployment artifacts.

The utility works both when invoked standalone and when added as a subdirectory of a larger CMake project. In standalone mode, `NS_BUILD_DIR` defaults to the parent of the postbuild build directory; in subdirectory mode, it defaults to the current binary directory unless the parent project overrides it.

### What it does

- Assembles the NS binary (e.g. `${PROJECT_NAME}.bin`) with the TF‑M Secure binary (e.g. `tfm_s.bin`) into a combined image (e.g. `${PROJECT_NAME}_tfm_s_ns.bin`).
- Signs the combined image into `${PROJECT_NAME}_tfm_s_ns_signed.bin` using the TF‑M signing keys.
- Copies output binaries and bootloader files into `${BASE_DIR}/bin/` for flashing or further use, typically including:
  - `${PROJECT_NAME}.bin`
  - `tfm_s_ns_signed.bin` (signed TF-M S/NS combined image)
  - `bl2.stm32`
  - `ddr_phy_signed.bin`
- Requires Python 3 to run the TF-M assembly/signing scripts.

### How to use

You can integrate the postbuild utility in different ways depending on your build flow.

#### 1. Standalone mode (manual / scripted)

Invoke the postbuild utility directly using CMake, passing the required variables (and optionally `NS_BUILD_DIR`):

- `PROJECT_NAME` — base name of your NS application binary (without extension).
- `TFM_BUILD_DIR` — path to the TF‑M build directory containing Secure images and keys.
- `NS_BUILD_DIR` — directory that contains the built NS artifacts (`${PROJECT_NAME}.elf/.bin`). Optional: in standalone mode this defaults to the parent of the postbuild build directory.
- `BASE_DIR` — output base directory where the `bin/` folder will be created (commonly the CM33 project root).
- `TFM_S_NS_SIGN_KEY` — optional override for the TF-M signing key. If the configured file is missing, postbuild falls back to the default key under `${TFM_BUILD_DIR}/api_ns/image_signing/keys/`.

Example:

```sh
cmake -G "Unix Makefiles" -S Utilities/M33TD_NSAppCore/postbuild -B build_post \
  -DPROJECT_NAME=<your_ns_project_name> \
  -DTFM_BUILD_DIR=<path-to-tfm-build> \
  -DNS_BUILD_DIR=<path-to-ns-build> \
  -DBASE_DIR=<your_project_root>

cmake --build build_post --target M33TD_NSAppCore_postbuild
```

This is suitable when invoking post-build from STM32CubeIDE, or when running from the command line.

If Python-based signing fails, first verify `TFM_BUILD_DIR`, `TFM_S_NS_SIGN_KEY`, and the signing-password variables used by your build wrapper. The current postbuild logic declares `TFM_S_NS_SIGN_KEY_PSWD` but passes `TFM_S_NS_SIGN_PSWD` to the wrapper script, so project wrappers should set the variable expected by the invocation path they use.

#### 2. Subdirectory mode (CMake integration)

Include the postbuild CMake project as a subdirectory of your main CMake project:

```cmake
add_subdirectory(Utilities/M33TD_NSAppCore/postbuild)

set(PROJECT_NAME <your_ns_project_name>)
set(TFM_BUILD_DIR <path-to-tfm-build>)
set(BASE_DIR <your_project_root>)

add_dependencies(<your_ns_target> M33TD_NSAppCore_postbuild)
```

- The parent project passes `PROJECT_NAME`, `TFM_BUILD_DIR`, and `BASE_DIR`.
- The `M33TD_NSAppCore_postbuild` target runs after the main application is built, so images are assembled and signed automatically as part of your normal build.

#### 3. STM32CubeIDE post‑build step

From STM32CubeIDE, you can call the postbuild utility as a post‑build step so that it runs automatically every time you build your CM33‑NS application.

Add a post‑build command similar to:

```sh
cmake -G "Unix Makefiles" \
  -S ../../../../../../../../Utilities/M33TD_NSAppCore/postbuild \
  -B M33TD_NSAppCore_postbuild \
  -DPROJECT_NAME=${BuildArtifactFileBaseName} \
  -DTFM_BUILD_DIR="<path-to-tfm-build>" \
  -DBASE_DIR=../../../../ \
  && cmake --build M33TD_NSAppCore_postbuild --target M33TD_NSAppCore_postbuild
```

- `${BuildArtifactFileBaseName}` is provided by CubeIDE and resolves to your NS artifact base name.
- The relative paths (`-S`, `-B`, `-DBASE_DIR`) should be adapted to your project’s folder layout.

### Steps performed internally

For details, see `postbuild/CMakeLists.txt`. In summary, the utility:

1. Assembles NS and Secure images using the provided Python scripts.
2. Signs the combined image with the TF‑M signing key.
3. Copies the resulting binaries and bootloader files into `${BASE_DIR}/bin/`.

You can customize:

- Input file names and locations.
- Output directory layout.
- Signing behavior and keys.

by adjusting the CMake variables passed to the postbuild project or by extending the logic in `postbuild/CMakeLists.txt`.

---

## Integration Verification Checklist

After integrating this stack into a project, verify the points below before treating the port as stable:

1. **Bootstrap and task profile**
   - Confirm `NSCoreApp_Init()` starts the task set you expect for the project profile.
   - Confirm project overrides in `nsappcore_config.h` and `app_tasks_config.h` match the intended `ENABLE_*` macros.

2. **Remote processor lifecycle**
   - Verify CA35 auto-start behavior matches `REMOTE_PROC_AUTO_START`.
   - Verify crash detection and stop/start recovery paths through RemoteProcTask.

3. **OpenAMP transport (when enabled)**
   - Verify the required RPMsg endpoints are created for the selected profile:
     - `power` at `0x58`
     - `display` at `0x59`
     - `low_power` at `0x5A`
     - `fwu` at `0x5B`
   - Verify the project-level OpenAMP integration forwards mailbox RX notifications into OpenAMPTask.

4. **Low-power behavior (when enabled)**
   - Verify SCMI suspend and shutdown notifications are routed into LowPowerMgrTask.
   - Verify policy control commands `LIMIT_PM_DISABLED`, `LIMIT_PM_STOP2`, `LIMIT_PM_LP_STOP2`, and `LIMIT_PM_LPLV_STOP2` are accepted over the low-power RPMsg endpoint.
   - Verify `LIMIT_PM_STANDBY` remains a recognized-but-ignored command unless the implementation changes.

5. **FWU flow (when enabled)**
   - Verify `ENABLE_FWU_MGR_TASK=1` is paired with `ENABLE_OPENAMP_TASK=1`.
   - Verify the FWU endpoint is registered and that the update flow can query component information and drive install/reboot handling.

6. **Display pipeline (when enabled)**
   - Verify DisplayTask initialization and any display-side RPMsg message handling used by the project profile.

7. **Postbuild outputs**
   - Verify `${BASE_DIR}/bin/` contains the expected raw NS binary, signed TF-M S/NS image, `bl2.stm32`, and `ddr_phy_signed.bin`.
   - If signing fails, verify both `TFM_S_NS_SIGN_KEY_PSWD` and `TFM_S_NS_SIGN_PSWD` in the build wrapper path you use.

---

## Reference projects

- `Firmware/Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD`  
  (feature-rich application reference)
- `Firmware/Projects/STM32MP257F-EV1/Demonstrations/DisplayDemo_M33TD`  
  (display-focused integration reference)
- `Firmware/Projects/STM32MP257F-EV1/Templates/Template_StarterApp_M33TD`  
  (bare-minimum starting point for STM32MP257 projects)
- `Firmware/Projects/STM32MP215F-DK/Templates/Template_StarterApp_M33TD`  
  (bare-minimum starting point for STM32MP215 projects)
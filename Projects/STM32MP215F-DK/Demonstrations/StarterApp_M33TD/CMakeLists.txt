cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "util/arm-gcc-toolchain.cmake")
set(BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/")
project(StarterApp_M33TD_CM33_NonSecure C ASM)

# Explicitly delete the build directory using cmake -E remove_directory
if(EXISTS "${CMAKE_BINARY_DIR}")
    message(STATUS "Deleting the build directory using cmake -E remove_directory...")
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}")
endif()

# === Build Configuration Options ===
# Define options for external paths
set(TFM_BUILD_DIR_DEFAULT "${BASE_DIR}/../../../../Middlewares/Third_Party/trusted-firmware-m/config_default")
set(TFM_BUILD_DIR "${TFM_BUILD_DIR_DEFAULT}" CACHE PATH "Path to the Trusted Firmware M directory")

# BUILD_CONFIG: FULL (default) or MINIMUM
set(BUILD_CONFIG "FULL" CACHE STRING "Build configuration: FULL or MINIMUM")
set_property(CACHE BUILD_CONFIG PROPERTY STRINGS FULL MINIMUM)

# BOOT_SPLASHSCREEN: DYNAMIC (default) or STATIC
set(BOOT_SPLASHSCREEN "DYNAMIC" CACHE STRING "Boot splash screen: DYNAMIC or STATIC")
set_property(CACHE BOOT_SPLASHSCREEN PROPERTY STRINGS DYNAMIC STATIC)

# Define the REMOTE_PROC_AUTO_START option
option(REMOTE_PROC_AUTO_START "Enable remote processor auto-start" ON)

# Define the default low power policy option
option(LOW_POWER_DEFAULT_POLICY_ENABLE "Enable default low power entry policy" OFF)

# Pass REMOTE_PROC_AUTO_START to the compiler as a 0/1 definition.
# This matches the config template behavior which treats the macro as numeric.
if(REMOTE_PROC_AUTO_START)
	add_definitions(-DREMOTE_PROC_AUTO_START=1)
else()
	add_definitions(-DREMOTE_PROC_AUTO_START=0)
endif()

if(LOW_POWER_DEFAULT_POLICY_ENABLE)
	add_definitions(-DLOW_POWER_DEFAULT_POLICY_ENABLE=1)
else()
	add_definitions(-DLOW_POWER_DEFAULT_POLICY_ENABLE=0)
endif()

# Option for REALTIME_DEBUG_LOG_ENABLED (enables REALTIME_DEBUG_LOG_ENABLED macro)
option(REALTIME_DEBUG_LOG_ENABLED "Enable real-time debug log (REALTIME_DEBUG_LOG_ENABLED macro)" OFF)

if(REALTIME_DEBUG_LOG_ENABLED)
	add_definitions(-DREALTIME_DEBUG_LOG_ENABLED=1)
else()
	add_definitions(-DREALTIME_DEBUG_LOG_ENABLED=0)
endif()

# Define options for FaultMgr feature toggles
option(FAULT_EXCEPTION_ENABLE "Enable Fault Exception Handler (FAULT_EXCEPTION_ENABLE macro)" ON)
option(FAULT_EXCEPTION_BACKTRACE_ENABLE "Enable FaultMgr backtrace (FAULT_EXCEPTION_BACKTRACE_ENABLE macro)" ON)

if(FAULT_EXCEPTION_ENABLE)
	add_definitions(-DFAULT_EXCEPTION_ENABLE=1)
else()
	add_definitions(-DFAULT_EXCEPTION_ENABLE=0)
endif()

if(FAULT_EXCEPTION_BACKTRACE_ENABLE)
	add_definitions(-DFAULT_EXCEPTION_BACKTRACE_ENABLE=1)
else()
	add_definitions(-DFAULT_EXCEPTION_BACKTRACE_ENABLE=0)
endif()

set(TFM_PLATFORM_S_DEVICETREE_DIR "${TFM_BUILD_DIR}/generated/devicetree/platform_s")
if(NOT EXISTS "${TFM_PLATFORM_S_DEVICETREE_DIR}")
	set(TFM_PLATFORM_S_DEVICETREE_DIR "${TFM_BUILD_DIR}/build-spe/generated/devicetree/platform_s")
endif()

message("========================================")
message("         CMake Configuration")
message("========================================")
message("Project Name: ${PROJECT_NAME}")
message("Toolchain File: ${CMAKE_TOOLCHAIN_FILE}")
message("Build Directory: ${CMAKE_BINARY_DIR}")
message("========================================")
message("Enabled Options:")
message("  BUILD_CONFIG: ${BUILD_CONFIG}")
message("  BOOT_SPLASHSCREEN: ${BOOT_SPLASHSCREEN}")
message("  REMOTE_PROC_AUTO_START: ${REMOTE_PROC_AUTO_START}")
message("  LOW_POWER_DEFAULT_POLICY_ENABLE: ${LOW_POWER_DEFAULT_POLICY_ENABLE}")
message("  FAULT_EXCEPTION_ENABLE: ${FAULT_EXCEPTION_ENABLE}")
if(BUILD_CONFIG STREQUAL "FULL")
    message("  Display Panel: ENABLED")
    if(BOOT_SPLASHSCREEN STREQUAL "DYNAMIC")
        message("  Splash Animation: ENABLED")
    else()
        message("  Splash Animation: DISABLED (STATIC)")
    endif()
else()
    message("  Display Panel: DISABLED (MINIMUM BUILD)")
endif()
message("========================================")

# Debugging message
# message("-- TFM_BUILD_DIR: ${TFM_BUILD_DIR}")
# message("Signing key: ${TFM_S_NS_SIGN_KEY}")
# message("Signing key Pswd: ${TFM_S_NS_SIGN_KEY_PSWD_ARG}")

add_definitions() 
include(util/cortex-m33-stm32mp2.cmake)
include(util/color.cmake)
message("-- Starting Cmake compilation ${LINKER_SCRIPT} ${CMAKE_BINARY_DIR}")

set(DEFINITIONS "-DSTM32MP215Fxx \
				-DSTM32MP21xxxx \
				-DSTM32MP2\
				-DUSE_STM32MP215FXX_DK \
				-DCORE_CM33 \
		        -DUSE_HAL_TIM_REGISTER_CALLBACKS=1 \
				")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS} ${COMMON_COMPILE_FLAGS} -T${LINKER_SCRIPT} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${COMMON_COMPILE_FLAGS} -DSTM32MP215Fxx -DSTM32MP21xxxx")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")

# Include modular source files
include(${BASE_DIR}/cmake/hal_sources.cmake)
include(${BASE_DIR}/cmake/freertos_sources.cmake)

#include NSAppCore sources
include(${BASE_DIR}/cmake/nsappcore_sources.cmake)

# Include openamp library generations
include(${BASE_DIR}/cmake/openamp_freertos.cmake)

# === Display Panel Feature Control ===
set(DISPLAY_PANEL_FILES
	${BASE_DIR}/CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/display_driver_system.c
    ${BASE_DIR}/CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/display_panel_rocktech.c
	${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Common/display_utils.c
)

set(BSP_SRC_FILES ${BSP_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/BSP/STM32MP215F-DK/stm32mp215f_disco.c 
						)

set(UTILS_SRC_FILES ${UTILS_SRC_FILES}
						${BASE_DIR}/../../../../Utilities/ResourcesManager/res_mgr.c 
						)
set(CMSIS_SRC_FILES ${CMSIS_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/system_stm32mp2xx_m33_ns.c
						${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/gcc/startup_stm32mp21xx_m33_ns.S
						)

# Conditionally add LTDC driver only for FULL build
if(BUILD_CONFIG STREQUAL "FULL")
	list(APPEND HAL_SRC_FILES
		${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_ltdc.c
	)
endif()

# Set application source files
set(APPLICATION_SRC
	${HAL_SRC_FILES}
	${FREERTOS_SRC_FILES}
	${BSP_SRC_FILES}
	${UTILS_SRC_FILES}
	${CMSIS_SRC_FILES}
    ${NSAPPCORE_SRC_FILES}	
    ${BASE_DIR}/CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/led_driver.c
	${BASE_DIR}/CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/low_power_mgr_driver.c
    ${BASE_DIR}/CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/logger_output_driver.c
	${BASE_DIR}/CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/remoteproc_driver.c
    ${BASE_DIR}/CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/openamp_driver.c
	${BASE_DIR}/CM33/NonSecure/FREERTOS/M33TD_NSAppCore/AppDriver/btn_monitor_driver.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/main.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/syscalls.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_hal_timebase_tim.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_it.c
	${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/openamp.c
	${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/rsc_table.c
	${BASE_DIR}/CM33/NonSecure/TFM/App/tfm_ns_interface_freertos.c
	${TFM_BUILD_DIR}/api_ns/interface/src/tfm_tz_psa_ns_api.c
	${TFM_BUILD_DIR}/api_ns/interface/src/tfm_platform_api.c
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/src/tfm_ioctl_cpu_api.c
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/src/tfm_ioctl_wdt_api.c
	${TFM_BUILD_DIR}/api_ns/interface/src/tfm_ns_notif_api.c
	${TFM_BUILD_DIR}/api_ns/interface/src/tfm_fwu_api.c
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/src/tfm_pm_api.c
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/src/tfm_scmi_api.c
	${VIRT_UART_FILE}
)
						
set(S_VEENER_OBJECT_FILE ${TFM_BUILD_DIR}/api_ns/interface/lib/s_veneers.o)

# By default, always define DISPLAY_PANEL_ENABLED and include display files (FULL build)
if(BUILD_CONFIG STREQUAL "FULL")
	add_definitions(-DDISPLAY_PANEL_ENABLED=1)
	list(APPEND APPLICATION_SRC ${DISPLAY_PANEL_FILES})
	add_definitions(-DHAL_LTDC_MODULE_ENABLED)
	add_definitions(-DLCD_ORIENTATION_PORTRAIT)
	add_definitions(-DUSE_LCD_RGB_RK043FN48H)

	# Splash animation control (only effective in FULL build)
	if(BOOT_SPLASHSCREEN STREQUAL "DYNAMIC")
		add_definitions(-DSPLASH_ANIMATION_ENABLED=1)
	endif()
	# If STATIC, do not define SPLASH_ANIMATION_ENABLED
else()
	# MINIMUM build: do not define DISPLAY_PANEL_ENABLED, do not include display files
	# (No add_definitions for DISPLAY_PANEL_ENABLED)
	# (No display files in APPLICATION_SRC)
endif()

set(SOURCE_FILES ${APPLICATION_SRC} ${LINKER_SCRIPT} ${S_VEENER_OBJECT_FILE})

# Add executable
add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES} ${LINKER_SCRIPT})
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE LINUX_RPROC_MASTER)
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE ${OPENAMP_DEFINITIONS})

# Link libraries
target_link_libraries(${PROJECT_NAME}.elf PRIVATE metal-static open_amp-static)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
	${BASE_DIR}/CM33/NonSecure/Core/Inc
	${BASE_DIR}/CM33/NonSecure/FaultMgr
	${BASE_DIR}/CM33/NonSecure/FREERTOS/App
	${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include
	${BASE_DIR}/../../../../Drivers/CMSIS/Core/Include
	${BASE_DIR}/../../../../Utilities/ResourcesManager
	${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/App/Inc
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Includes
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Common	
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Common/FaultMgr
	$<$<STREQUAL:${BUILD_CONFIG},FULL>:${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Assets/anim_13fps_480x272>
	$<$<STREQUAL:${BUILD_CONFIG},FULL>:${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Common/Panel>
	$<$<STREQUAL:${BUILD_CONFIG},FULL>:${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/Assets>
	${BASE_DIR}/../../../../Drivers/BSP/STM32MP215F-DK
	${BASE_DIR}/../../../../Drivers/BSP/Components/Common
	${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Inc
	${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/include
	${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure
	${BASE_DIR}/../../../../Drivers/CMSIS/RTOS2/Include
	${BASE_DIR}/../../../../Drivers/CMSIS/RTOS2/Template
	${TFM_BUILD_DIR}/api_ns/interface/include
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/include/uapi
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/include
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/native_driver/include
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/cmsis_driver/include
	${TFM_BUILD_DIR}/api_ns/common/devicetree/include
	${TFM_PLATFORM_S_DEVICETREE_DIR}
	${LIBMETAL_BUILD_DIR_DEFAULT}/lib/include
	${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/include
	${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/virtual_driver/
	${BASE_DIR}/CM33/NonSecure/OpenAMP/Inc
)

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--gc-sections -static  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -z max-page-size=4096 -lc -lm -Wl,--end-group")

# Post-build commands
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
     COMMAND ${TOOLCHAIN_PREFIX}size --format=berkeley ${PROJECT_NAME}.elf
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -S ${PROJECT_NAME}.elf ${PROJECT_NAME}_stripped.elf
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# === Integrate Post-Build Utility from Common Stack ===
set(NS_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "Build dir")
add_subdirectory(
    ${BASE_DIR}/../../../../Utilities/M33TD_NSAppCore/postbuild
    ${CMAKE_BINARY_DIR}/M33TD_NSAppCore_postbuild
)
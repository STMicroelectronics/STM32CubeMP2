cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "util/arm-gcc-toolchain.cmake")
set(BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/")
set(CMAKE_BINARY_DIR "${BASE_DIR}build")
project(StarterApp_M33TD_CM33_NonSecure C ASM)

# Explicitly delete the build directory using cmake -E remove_directory
if(EXISTS "${CMAKE_BINARY_DIR}")
    message(STATUS "Deleting the build directory using cmake -E remove_directory...")
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}")
endif()
# Define options for external paths
set(TFM_BUILD_DIR_DEFAULT "${BASE_DIR}/../../../../Middlewares/Third_Party/trusted-firmware-m/config_default")
set(TFM_BUILD_DIR "${TFM_BUILD_DIR_DEFAULT}" CACHE PATH "Path to the Trusted Firmware M directory")

# Define the REMOTE_PROC_AUTO_START option
option(REMOTE_PROC_AUTO_START "Enable remote processor auto-start" ON)

# Pass REMOTE_PROC_AUTO_START to the compiler as a definition
if(REMOTE_PROC_AUTO_START)
    add_definitions(-DREMOTE_PROC_AUTO_START)
endif()

message("========================================")
message("         CMake Configuration")
message("========================================")
message("Project Name: ${PROJECT_NAME}")
message("Toolchain File: ${CMAKE_TOOLCHAIN_FILE}")
message("Build Directory: ${CMAKE_BINARY_DIR}")
message("========================================")
message("Enabled Options:")
message("  REMOTE_PROC_AUTO_START: ${REMOTE_PROC_AUTO_START}")
message("========================================")

# Debugging message
# message("-- TFM_BUILD_DIR: ${TFM_BUILD_DIR}")


add_definitions() 
include(util/cortex-m33-stm32mp2.cmake)
include(util/color.cmake)
message("-- Starting Cmake compilation ${LINKER_SCRIPT} ${CMAKE_BINARY_DIR}")

set(DEFINITIONS "-DSTM32MP257Cxx \
				-DSTM32MP25xxxx \
				-DSTM32MP2\
				-DUSE_STM32MP257F_EV1 \
				-DCORE_CM33 \
		        -DUSE_HAL_TIM_REGISTER_CALLBACKS=1 \
				")

set(TARGET StarterApp_M33TD_CM33_NonSecure)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS} ${COMMON_COMPILE_FLAGS} -T${LINKER_SCRIPT} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${COMMON_COMPILE_FLAGS} -DSTM32MP257Cxx -DSTM32MP25xxxx")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")

# Include modular source files
include(${BASE_DIR}/cmake/hal_sources.cmake)
include(${BASE_DIR}/cmake/freertos_sources.cmake)

set(BSP_SRC_FILES ${BSP_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/BSP/STM32MP257F-EV1/stm32mp257f_eval.c 
						)

set(UTILS_SRC_FILES ${UTILS_SRC_FILES}
						${BASE_DIR}/../../../../Utilities/ResourcesManager/res_mgr.c 
						)
set(CMSIS_SRC_FILES ${CMSIS_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/system_stm32mp2xx_m33_ns.c
						${BASE_DIR}/startup_stm32mp257cxx_m33.s
						)
# Set application source files
set(APPLICATION_SRC 
	${HAL_SRC_FILES} 
	${FREERTOS_SRC_FILES} 
	${BSP_SRC_FILES} 
	${UTILS_SRC_FILES} 
	${CMSIS_SRC_FILES}
	${BASE_DIR}/CM33/NonSecure/FREERTOS/App/app_freertos.c
	${BASE_DIR}/CM33/NonSecure/FREERTOS/App/remoteproc_task.c
	${BASE_DIR}/CM33/NonSecure/FREERTOS/App/userapp_task.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/main.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/syscalls.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_hal_timebase_tim.c
	${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_it.c
	${BASE_DIR}/CM33/NonSecure/TFM/App/tfm_ns_interface.c
	${TFM_BUILD_DIR}/api_ns/interface/src/tfm_tz_psa_ns_api.c
	${TFM_BUILD_DIR}/api_ns/interface/src/tfm_platform_api.c
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/src/tfm_ioctl_cpu_api.c
)
						
set(S_VEENER_OBJECT_FILE ${TFM_BUILD_DIR}/api_ns/interface/lib/s_veneers.o)

include_directories(
	${BASE_DIR}/CM33/NonSecure/Core/Inc
	${BASE_DIR}/CM33/NonSecure/FREERTOS/App
	${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include
	${BASE_DIR}/../../../../Drivers/CMSIS/Core/Include
	${BASE_DIR}/../../../../Utilities/ResourcesManager
	${BASE_DIR}/../../../../Drivers/BSP/STM32MP257F-EV1
	${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Inc
	${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/include
	${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure
	${BASE_DIR}/../../../../Drivers/CMSIS/RTOS2/Include
	${BASE_DIR}/../../../../Drivers/CMSIS/RTOS2/Template
	${TFM_BUILD_DIR}/api_ns/interface/include
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/include/uapi
	${TFM_BUILD_DIR}/api_ns/common/stm32mp2xx/secure/services/include
)

set(SOURCE_FILES ${APPLICATION_SRC} ${LINKER_SCRIPT} ${S_VEENER_OBJECT_FILE})

# Add executable
add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES} ${LINKER_SCRIPT})

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--gc-sections -static  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -z max-page-size=4096 -lc -lm -Wl,--end-group")

# Post-build commands
add_custom_command(TARGET ${TARGET}.elf POST_BUILD
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Oihex ${TARGET}.elf ${TARGET}.hex
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Obinary ${TARGET}.elf ${TARGET}.bin
     COMMAND ${TOOLCHAIN_PREFIX}size --format=berkeley ${TARGET}.elf
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -S ${TARGET}.elf ${TARGET}_stripped.elf
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

find_package(Python3)

# Assemble and sign the binaries
add_custom_command(
	TARGET ${PROJECT_NAME}.elf POST_BUILD
	COMMAND ${Python3_EXECUTABLE} ${TFM_BUILD_DIR}/api_ns/image_signing/scripts/assemble.py
		--layout ${TFM_BUILD_DIR}/api_ns/image_signing/layout_files/signing_layout_s_ns.o
		--secure ${TFM_BUILD_DIR}/api_ns/bin/tfm_s.bin
		--non_secure ${TARGET}.bin
		--output ${TARGET}_tfm_s_ns.bin
	COMMAND ${Python3_EXECUTABLE} ${TFM_BUILD_DIR}/api_ns/image_signing/scripts/wrapper/wrapper.py
		--version 2.1.0
		--layout ${TFM_BUILD_DIR}/api_ns/image_signing/layout_files/signing_layout_s_ns.o
		--key ${TFM_BUILD_DIR}/api_ns/image_signing/keys/image_s_signing_private_key.pem
		--public-key-format full
		--align 1
		--pad
		--pad-header -H 0x800
		-s 1
		-L 128
		--measured-boot-record ${TARGET}_tfm_s_ns.bin
		${TARGET}_tfm_s_ns_signed.bin
	COMMENT "Assembling and Signing the binaries"
	VERBATIM
)

# Copy flash binaries to the bin folder
add_custom_command(
	TARGET ${PROJECT_NAME}.elf POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${TARGET}_tfm_s_ns_signed.bin ${BASE_DIR}/bin/tfm_s_ns_signed.bin
	COMMAND ${CMAKE_COMMAND} -E copy ${TARGET}.bin ${BASE_DIR}/bin/${TARGET}.bin
	COMMAND ${CMAKE_COMMAND} -E copy ${TFM_BUILD_DIR}/api_ns/bin/bl2.stm32 ${BASE_DIR}/bin/bl2.stm32
	COMMAND ${CMAKE_COMMAND} -E copy ${TFM_BUILD_DIR}/api_ns/bin/ddr_phy_signed.bin ${BASE_DIR}/bin/ddr_phy_signed.bin
	COMMENT "Copying flash binaries in bin folder"
	VERBATIM
)

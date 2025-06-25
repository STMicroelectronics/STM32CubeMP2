cmake_minimum_required(VERSION 3.10)

# NOTE: path to the linker script is defined in cortex-m0plus-stm32mp2.cmake
set(CMAKE_TOOLCHAIN_FILE "util/arm-gcc-toolchain.cmake")
set(BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/")
set(CMAKE_BINARY_DIR "${BASE_DIR}build")

project(CM0PLUS_DEMO_NonSecure C ASM)

add_definitions() 
include(util/cortex-m0plus-stm32mp2.cmake)
include(util/color.cmake)
message("-- Starting Cmake compilation ${LINKER_SCRIPT} ${CMAKE_BINARY_DIR}")

set(DEFINITIONS "-DSTM32MP257Cxx \
				-DCORE_CM0PLUS \
				-DSTM32MP257F_EV1 \
				-DSTM32MP25xx_EVAL \
				-DSTM32MP25xxxx \
				-DUSE_HAL_DRIVER \
				-DUSE_FULL_LL_DRIVER \
				-DLOWPOWER_WA_STANDBY1 \
				-DSTM32MP2 \
				-DUSE_STM32MP257F_EV1")

set(TARGET CM0PLUS_DEMO_NonSecure)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS} ${COMMON_COMPILE_FLAGS} -T${LINKER_SCRIPT} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${COMMON_COMPILE_FLAGS} -DSTM32MP257Cxx -DSTM32MP25xxxx")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
set(HAL_SRC_FILES ${HAL_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_cortex.c
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_gpio.c
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_hsem.c
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_ipcc.c
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal.c
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_pwr_ex.c
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_pwr.c
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_uart_ex.c
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_uart.c
						)

set(BSP_SRC_FILES ${BSP_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/BSP/STM32MP257F-EV1/stm32mp257f_eval.c  
						)
set(UTILS_SRC_FILES ${UTILS_SRC_FILES}
						${BASE_DIR}/../../../../Utilities/ResourcesManager/res_mgr.c 
						${BASE_DIR}/../../../../Utilities/CoproSync/copro_sync.c
						)

# Set application source files
set(APPLICATION_SRC ${APPLICATION_SRC} ${HAL_SRC_FILES} ${BSP_SRC_FILES} ${UTILS_SRC_FILES}
						${BASE_DIR}/CM0PLUS/NonSecure/Core/Src/main.c /
						${BASE_DIR}/CM0PLUS/NonSecure/Core/Src/syscalls.c /
						${BASE_DIR}/CM0PLUS/NonSecure/Core/Src/stm32mp2xx_hal_msp.c /
						${BASE_DIR}/CM0PLUS/NonSecure/Core/Src/stm32mp2xx_it.c /
						${BASE_DIR}/CM0PLUS/NonSecure/Core/Src/lowpower_wa.c /
						${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/gcc/startup_stm32mp2xx_m0plus.s /
						${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/system_stm32mp2xx_m0plus.c /
						)

include_directories(APPLICATION_INCLUDE ${BASE_DIR}/CM0PLUS/NonSecure/Core/Inc )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/CMSIS/Include )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Utilities/CoproSync )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Utilities/ResourcesManager )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/BSP/STM32MP257F-EV1 )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Inc )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Inc/Legacy )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include/Templates_TZ )

set(SOURCE_FILES ${APPLICATION_SRC} ${LINKER_SCRIPT} )
add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES} ${LINKER_SCRIPT})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--gc-sections -static -mfloat-abi=soft -mthumb -Wl,--start-group -z max-page-size=4096 -lc -lm -Wl,--end-group")
add_custom_command(TARGET ${TARGET}.elf POST_BUILD
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Oihex ${TARGET}.elf ${TARGET}.hex
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Obinary ${TARGET}.elf ${TARGET}.bin
     COMMAND ${TOOLCHAIN_PREFIX}size --format=berkeley ${TARGET}.elf
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -S ${TARGET}.elf ${TARGET}_stripped.elf
	 COMMAND ${TOOLCHAIN_PREFIX}objdump -h -d ${TARGET}.elf  >> ${TARGET}.lst
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

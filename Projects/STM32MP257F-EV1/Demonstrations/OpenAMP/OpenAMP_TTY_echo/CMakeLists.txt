cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "util/arm-gcc-toolchain.cmake")
set(BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/")
set(CMAKE_BINARY_DIR "${BASE_DIR}build")
project(OpenAMP_TTY_echo_CM33_NonSecure C ASM)

add_definitions()
include(util/cortex-m33-stm32mp2.cmake)
include(util/color.cmake)
find_package(Python3 REQUIRED)
message("-- Starting Cmake compilation ${LINKER_SCRIPT} ${CMAKE_BINARY_DIR}")

set(DEFINITIONS "-DSTM32MP257Cxx \
				-DICACHE_DCACHE_USE \
				-DSTM32MP25xxxx \
				-DCORE_CM33 \
				-DUSE_STM32MP257F_EV1 \
				-DUSE_HAL_DRIVER \
				-DMETAL_INTERNAL \
				-DMETAL_MAX_DEVICE_REGIONS=2 \
				-DNO_ATOMIC_64_SUPPORT \
				-DVQ_RX_EMPTY_NOTIFY=0 \
				-DVIRTIO_DEVICE_SUPPORT=1 \
				-DVIRTIO_DRIVER_SUPPORT=0 \
				-DLINUX_RPROC_MASTER")

set(TARGET OpenAMP_TTY_echo_CM33_NonSecure)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_FLAGS} -T${LINKER_SCRIPT} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${COMMON_COMPILE_FLAGS} -DSTM32MP257Cxx")

set(HAL_SRC_FILES
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_cortex.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_dcache.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_dma.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_dma_ex.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_exti.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_gpio.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_icache.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_ipcc.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_pwr.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_pwr_ex.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_rcc.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_rcc_ex.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_uart.c
				${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_uart_ex.c)

set(OPENAMP_SRC_FILES
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/virtual_driver/virt_uart.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/remoteproc/remoteproc_virtio.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/rpmsg/rpmsg.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/rpmsg/rpmsg_virtio.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/utils/utilities.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/virtio/virtio.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/virtio/virtqueue.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/device.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/init.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/io.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/log.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/shmem.c)

set(BSP_SRC_FILES
				${BASE_DIR}/../../../../../Drivers/BSP/STM32MP257F-EV1/stm32mp257f_eval.c)

set(UTILS_SRC_FILES
				${BASE_DIR}/../../../../../Utilities/CoproSync/copro_sync.c
				${BASE_DIR}/../../../../../Utilities/ResourcesManager/res_mgr.c)

set(CMSIS_SRC_FILES
				${BASE_DIR}/../../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/system_stm32mp2xx_m33_ns.c)

set(OPENAMP_SRC_GEN_FILES
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/generic/generic_device.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/generic/generic_io.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/generic/generic_init.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/generic/generic_shmem.c
				${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/generic/template/sys.c)

set(APPLICATION_SRC ${HAL_SRC_FILES} ${OPENAMP_SRC_FILES} ${OPENAMP_SRC_GEN_FILES} ${BSP_SRC_FILES} ${UTILS_SRC_FILES} ${CMSIS_SRC_FILES}
				${BASE_DIR}/CM33/NonSecure/Core/Src/main.c
				${BASE_DIR}/../../../../../Utilities/Services/GenericServices/syscalls.c
				${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c
				${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_it.c
				${BASE_DIR}/startup_stm32mp257cxx_m33.s
				${BASE_DIR}/CM33/NonSecure/OPENAMP/mbox_ipcc.c
				${BASE_DIR}/CM33/NonSecure/OPENAMP/openamp.c
				${BASE_DIR}/CM33/NonSecure/OPENAMP/openamp_log.c
				${BASE_DIR}/CM33/NonSecure/OPENAMP/rsc_table.c)

include_directories(APPLICATION_INCLUDE ${BASE_DIR}/CM33/NonSecure/Core/Inc)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/CM33/NonSecure/OPENAMP)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../../Drivers/CMSIS/Core/Include)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../../Utilities/CoproSync)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../../Utilities/ResourcesManager)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../../Drivers/BSP/STM32MP257F-EV1)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../../Drivers/STM32MP2xx_HAL_Driver/Inc)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/include)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/build/libmetal/lib/include)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/virtual_driver)

set(SOURCE_FILES ${APPLICATION_SRC} ${LINKER_SCRIPT})
add_custom_target(openamp_gen
    COMMAND ${CMAKE_COMMAND} -S ${BASE_DIR}/../../../../../Middlewares/Third_Party/OpenAMP/libmetal/ -B libmetal -G'Unix Makefiles' -DCMAKE_TOOLCHAIN_FILE=cmake/platforms/stm32mp-generic-gcc.cmake
)
add_custom_command(
  OUTPUT ${OPENAMP_SRC_GEN_FILES}
  COMMAND openamp_gen
  COMMENT "Generating the source file for openamp."
  VERBATIM
)
add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES} ${LINKER_SCRIPT})
add_dependencies(${PROJECT_NAME}.elf openamp_gen)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--gc-sections -static -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -z max-page-size=4096 -lc -lm -Wl,--end-group")
add_custom_command(TARGET ${TARGET}.elf POST_BUILD
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Oihex ${TARGET}.elf ${TARGET}.hex
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Obinary ${TARGET}.elf ${TARGET}.bin
     COMMAND ${TOOLCHAIN_PREFIX}size --format=berkeley ${TARGET}.elf
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -S ${TARGET}.elf ${TARGET}_stripped.elf
     COMMAND ${Python3_EXECUTABLE} ${BASE_DIR}/../../../../../Utilities/optee_os/scripts/sign_rproc_fw.py
             --in ${TARGET}_stripped.elf
             --out ${TARGET}_sign.bin
             --key ${BASE_DIR}/../../../../../Utilities/optee_os/keys/default.pem
             --plat-tlv BOOTADDR 0x80100000
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
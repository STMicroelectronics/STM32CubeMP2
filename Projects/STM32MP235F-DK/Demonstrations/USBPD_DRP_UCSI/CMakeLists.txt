cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "util/arm-gcc-toolchain.cmake")
set(BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/")
set(CMAKE_BINARY_DIR "${BASE_DIR}build")
project(USBPD_DRP_UCSI_CM33_NonSecure C ASM)

add_definitions() 
include(util/cortex-m33-stm32mp2.cmake)
include(util/color.cmake)
message("-- Starting Cmake compilation ${LINKER_SCRIPT} ${CMAKE_BINARY_DIR}")

set(DEFINITIONS "-DSTM32MP235Fxx \
				-DSTM32MP23xxxx \
				-DLOGLEVEL=4 \
				-DUSE_FULL_LL_DRIVER \
				-DTX_SINGLE_MODE_NON_SECURE=1 \
				-DTX_INCLUDE_USER_DEFINE_FILE \
				-D__LOG_UART_IO_ \
				-DUSE_STM32MP235F_DK \
				-DNO_ATOMIC_64_SUPPORT \
				-DMETAL_INTERNAL \
				-DVIRTIO_DEVICE_ONLY \
				-DMETAL_MAX_DEVICE_REGIONS=2 \
				-DLINUX_RPROC_MASTER \
				-DCORE_CM33 \
				-DUSE_HAL_DRIVER \
				-DTCPP0203_SUPPORT \
				-DUSBPD_PORT_COUNT=1 \
				-D_DRP \
				-D_TRACE \
				-DUSBPDCORE_LIB_PD3_FULL \
				-D_SRC_CAPA_EXT \
				-D_RTOS \
				-D_VCONN_SUPPORT")

set(TARGET USBPD_DRP_UCSI_CM33_NonSecure)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS} ${COMMON_COMPILE_FLAGS} -T${LINKER_SCRIPT} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} ${WARNING_FLAGS} ${DEFINITIONS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${COMMON_COMPILE_FLAGS} -DSTM32MP235Fxx -DSTM32MP23xxxx -DTX_SINGLE_MODE_NON_SECURE=1")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
set(HAL_SRC_FILES ${HAL_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_adc.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_adc_ex.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_cortex.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_dma.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_dma_ex.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_exti.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_gpio.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_i2c.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_i2c_ex.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_icache.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_ipcc.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_pwr.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_pwr_ex.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_rcc.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_rcc_ex.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_tim.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_tim_ex.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_hal_uart.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_ll_dma.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_ll_exti.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_ll_gpio.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_ll_rcc.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_ll_ucpd.c 
						${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Src/stm32mp2xx_ll_usart.c 
						)
set(OPENAMP_SRC_FILES ${OPENAMP_SRC_FILES}
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/virtual_driver/virt_i2c.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/virtual_driver/virt_intc.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/virtual_driver/virt_uart.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/remoteproc/remoteproc_virtio.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/rpmsg/rpmsg.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/rpmsg/rpmsg_virtio.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/virtio/virtio.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/virtio/virtqueue.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/freertos/condition.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/freertos/time.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/freertos/template/sys.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/device.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/init.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/io.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/log.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/shmem.c 
						)
set(OPENAMP_SRC_GEN_FILES ${OPENAMP_SRC_GEN_FILES}
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/freertos/freertos_device.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/freertos/freertos_io.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/freertos/freertos_init.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/system/freertos/freertos_shmem.c 
						)
set(FREERTOS_SRC_FILES ${FREERTOS_SRC_FILES}
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/croutine.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/event_groups.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/list.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/queue.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/tasks.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/timers.c 
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
						${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
						)
set(BSP_SRC_FILES ${BSP_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/BSP/STM32MP235F-DK/stm32mp235f_disco.c
						${BASE_DIR}/../../../../Drivers/BSP/STM32MP235F-DK/stm32mp235f_disco_bus.c
						${BASE_DIR}/../../../../Drivers/BSP/STM32MP235F-DK/stm32mp235f_disco_pmic.c
						${BASE_DIR}/../../../../Drivers/BSP/STM32MP235F-DK/stm32mp235f_disco_usbpd_pwr.c
						${BASE_DIR}/../../../../Drivers/BSP/Components/tcpp0203/tcpp0203.c 
						${BASE_DIR}/../../../../Drivers/BSP/Components/tcpp0203/tcpp0203_reg.c 
						)
set(USBPDLIB_SRC_FILES
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/UCSI/ppm.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/UCSI/usbpd_ucsi_ppm.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Devices/STM32MP2XX/src/usbpd_cad_hw_if.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Devices/STM32MP2XX/src/usbpd_hw.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Devices/STM32MP2XX/src/usbpd_hw_if_it.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Devices/STM32MP2XX/src/usbpd_phy.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Devices/STM32MP2XX/src/usbpd_phy_hw_if.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Devices/STM32MP2XX/src/usbpd_pwr_hw_if.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Devices/STM32MP2XX/src/usbpd_timersserver.c 
						${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Core/src/usbpd_trace.c 
						)
set(UTILS_SRC_FILES ${UTILS_SRC_FILES}
						${BASE_DIR}/../../../../Utilities/TRACER_EMB/tracer_emb.c 
						${BASE_DIR}/../../../../Utilities/ResourcesManager/res_mgr.c 
						${BASE_DIR}/../../../../Utilities/CoproSync/copro_sync.c
						${BASE_DIR}/../../../../Utilities/TRACER_EMB/tracer_emb_hw.c
						)
set(CMSIS_SRC_FILES ${CMSIS_SRC_FILES}
						${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/system_stm32mp2xx_m33_ns.c
						${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/gcc/startup_stm32mp235fxx_m33.s
						)
# Set application source files
set(APPLICATION_SRC ${APPLICATION_SRC} ${HAL_SRC_FILES} ${OPENAMP_SRC_FILES} ${OPENAMP_SRC_GEN_FILES} ${FREERTOS_SRC_FILES} ${BSP_SRC_FILES} ${USBPDLIB_SRC_FILES} ${UTILS_SRC_FILES} ${CMSIS_SRC_FILES}
						${BASE_DIR}/CM33/NonSecure/FREERTOS/App/app_freertos.c
						${BASE_DIR}/CM33/NonSecure/Core/Src/led_thread.c 
						${BASE_DIR}/CM33/NonSecure/Core/Src/main.c 
						${BASE_DIR}/CM33/NonSecure/Core/Src/syscalls.c
						${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_hal_msp.c 
						${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_hal_timebase_tim.c 
						${BASE_DIR}/CM33/NonSecure/Core/Src/stm32mp2xx_it.c 
						${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/mbox_ipcc.c 
						${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/openamp.c 
						${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/openamp_log.c 
						${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/openamp_thread.c 
						${BASE_DIR}/CM33/NonSecure/OpenAMP/Src/rsc_table.c 
						${BASE_DIR}/CM33/NonSecure/USBPD/App/usbpd.c 
						${BASE_DIR}/CM33/NonSecure/USBPD/App/usbpd_dpm_core.c 
						${BASE_DIR}/CM33/NonSecure/USBPD/App/usbpd_pwr_if.c 
						${BASE_DIR}/CM33/NonSecure/USBPD/App/usbpd_ucsi_if.c 
						${BASE_DIR}/CM33/NonSecure/USBPD/Target/usbpd_dpm_user.c 
						${BASE_DIR}/CM33/NonSecure/USBPD/Target/usbpd_pwr_user.c 
						${BASE_DIR}/CM33/NonSecure/USBPD/Target/usbpd_vdm_user.c)

include_directories(APPLICATION_INCLUDE ${BASE_DIR}/CM33/NonSecure/Core/Inc) 
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/CM33/NonSecure/USBPD/App )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/CM33/NonSecure/USBPD/Target )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/CM33/NonSecure/OpenAMP/Inc )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/CM33/NonSecure/FREERTOS/App )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/CMSIS/Core/Include )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Utilities/TRACER_EMB )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Utilities/CoproSync )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Utilities/ResourcesManager )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/BSP/STM32MP235F-DK )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/BSP/Components/stpmic2 )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/STM32MP2xx_HAL_Driver/Inc )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp/lib/include )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Core/inc )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Devices/STM32MP2XX/inc )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/UCSI )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/build/libmetal/lib/include )
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/virtual_driver)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/include)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/CMSIS/RTOS2/Include)
include_directories(APPLICATION_INCLUDE ${BASE_DIR}/../../../../Drivers/CMSIS/RTOS2/Template)

set(SOURCE_FILES ${APPLICATION_SRC} ${LINKER_SCRIPT} )
add_custom_target(openamp_gen
	COMMAND ${CMAKE_COMMAND} -S ${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/ -B libmetal  -G'Unix Makefiles' -DCMAKE_TOOLCHAIN_FILE=cmake/platforms/stm32mp-freertos-gcc.cmake
)
add_custom_command(
  OUTPUT ${OPENAMP_SRC_GEN_FILES}
  COMMAND openamp_gen
  COMMENT "Generating the source file for openamp."
  VERBATIM
)
link_directories(${CMAKE_SOURCE_DIR}/../../../../Middlewares/ST/STM32_USBPD_Library/Core/lib)
add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES} ${LINKER_SCRIPT})
add_dependencies(${PROJECT_NAME}.elf openamp_gen)
target_link_libraries(${PROJECT_NAME}.elf :USBPDCORE_PD3_FULL_CM33_wc32.a)
#target_link_libraries(${PROJECT_NAME}.elf PUBLIC)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--gc-sections -static  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -z max-page-size=4096 -lc -lm -Wl,--end-group")
add_custom_command(TARGET ${TARGET}.elf POST_BUILD
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Oihex ${TARGET}.elf ${TARGET}.hex
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -Obinary ${TARGET}.elf ${TARGET}.bin
     COMMAND ${TOOLCHAIN_PREFIX}size --format=berkeley ${TARGET}.elf
     COMMAND ${TOOLCHAIN_PREFIX}objcopy -S ${TARGET}.elf ${TARGET}_stripped.elf
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

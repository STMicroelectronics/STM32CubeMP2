set(LINKER_SCRIPT "${BASE_DIR}/../../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Source/Templates/gcc/linker/stm32mp2xx_DDR_m33_ns.ld")
set(WARNING_FLAGS "-Wall")
set(CPU_OPTIONS -mthumb -mcpu=cortex-m33)
set(CPU "cortex-m33")
set(CPU_OPTIONS "-mcpu=${CPU} -mthumb")
set(COMMON_COMPILE_FLAGS "${CPU_OPTIONS} -ffunction-sections -g3 -fstack-usage --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -Os")

include (cross-freertos-gcc)

set (CMAKE_SYSTEM_PROCESSOR "arm"            CACHE STRING "")
set (MACHINE                "template"       CACHE STRING "")
set (CROSS_PREFIX           "arm-none-eabi-" CACHE STRING "")

# Duplicate name for IAR IDE limitation: it can not create a project with sources files with the same name
if (LIBMETAL_ROOT_DIR)
  configure_file(${CMAKE_SOURCE_DIR}/lib/system/${PROJECT_SYSTEM}/device.c
    ${CMAKE_SOURCE_DIR}/lib/system/${PROJECT_SYSTEM}/${PROJECT_SYSTEM}_device.c COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/lib/system/${PROJECT_SYSTEM}/init.c
    ${CMAKE_SOURCE_DIR}/lib/system/${PROJECT_SYSTEM}/${PROJECT_SYSTEM}_init.c COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/lib/system/${PROJECT_SYSTEM}/shmem.c
    ${CMAKE_SOURCE_DIR}/lib/system/${PROJECT_SYSTEM}/${PROJECT_SYSTEM}_shmem.c COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/lib/system/${PROJECT_SYSTEM}/io.c
    ${CMAKE_SOURCE_DIR}/lib/system/${PROJECT_SYSTEM}/${PROJECT_SYSTEM}_io.c COPYONLY)
endif (LIBMETAL_ROOT_DIR)


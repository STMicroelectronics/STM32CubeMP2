include (cross-generic-gcc)

set (CMAKE_SYSTEM_PROCESSOR "arm"            CACHE STRING "")
set (MACHINE                "template"       CACHE STRING "")
set (CROSS_PREFIX           "arm-none-eabi-" CACHE STRING "")

# Duplicate name for IAR IDE limitation: it can not create a project with sources files with the same name
if (LIBMETAL_ROOT_DIR)
  configure_file(${CMAKE_SOURCE_DIR}/lib/system/generic/device.c ${CMAKE_SOURCE_DIR}/lib/system/generic/generic_device.c COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/lib/system/generic/init.c ${CMAKE_SOURCE_DIR}/lib/system/generic/generic_init.c COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/lib/system/generic/io.c ${CMAKE_SOURCE_DIR}/lib/system/generic/generic_io.c COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/lib/system/generic/shmem.c ${CMAKE_SOURCE_DIR}/lib/system/generic/generic_shmem.c COPYONLY)
endif (LIBMETAL_ROOT_DIR)


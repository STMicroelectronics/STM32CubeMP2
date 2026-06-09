set(LIBMETAL_BUILD_DIR_DEFAULT "${CMAKE_BINARY_DIR}/libmetal_build/config_default")
set(LIBMETAL_BUILD_DIR "${LIBMETAL_BUILD_DIR_DEFAULT}" CACHE PATH "Build path of the Libmetal library")

set(OPENAMP_BUILD_DIR_DEFAULT "${CMAKE_BINARY_DIR}/openamp_build/config_default")
set(OPENAMP_BUILD_DIR "${OPENAMP_BUILD_DIR_DEFAULT}" CACHE PATH "Build path of the open-amp library")


set(OPENAMP_DEFINITIONS
    METAL_MAX_DEVICE_REGIONS=2
    WITH_VIRTIO_DEVICE=ON
    WITH_VIRTIO_DRIVER=OFF
    WITH_LIBMETAL_FIND=OFF
    WITH_PROXY=OFF
    WITH_DOC=OFF
    NO_ATOMIC_64_SUPPORT
)


# Add libmetal library
string (TOLOWER "FreeRTOS"                PROJECT_SYSTEM)
string (TOUPPER "FreeRTOS"                PROJECT_SYSTEM_UPPER)
set(COMMON_DEFINITIONS METAL_MAX_DEVICE_REGIONS=2)
set(MACHINE                "template"       CACHE STRING "")
set(WITH_STATIC_LIB ON CACHE BOOL "" FORCE)
set(WITH_DOC OFF CACHE BOOL "" FORCE)

find_path (HAVE_STDATOMIC_H stdatomic.h)
find_path (HAVE_FCNTL_H fcntl.h)

add_subdirectory(${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal ${LIBMETAL_BUILD_DIR})
target_compile_definitions(metal-static PRIVATE ${OPENAMP_DEFINITIONS})

target_include_directories(metal-static PRIVATE
    ${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/include
    ${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure
    ${BASE_DIR}/CM33/NonSecure/FREERTOS/App
)

# Add openamp library
# set(WITH_VIRTIO_DEVICE 1 CACHE STRING "" FORCE)
# set(WITH_VIRTIO_DRIVER OFF CACHE BOOL "" FORCE)
# set(WITH_LIBMETAL_FIND OFF CACHE BOOL "" FORCE)
# set(WITH_PROXY OFF CACHE BOOL "" FORCE)
# set(NO_ATOMIC_64_SUPPORT CACHE BOOL "" FORCE)




set(LIBMETAL_INCLUDE_DIR ${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/include)
set(LIBMETAL_LIB ${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/libmetal/lib/)

add_subdirectory(${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/open-amp ${OPENAMP_BUILD_DIR})
target_compile_definitions(metal-static PRIVATE ${OPENAMP_DEFINITIONS})
target_include_directories(open_amp-static PRIVATE
    ${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/include
    ${BASE_DIR}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure
    ${BASE_DIR}/CM33/NonSecure/FREERTOS/App
    ${LIBMETAL_BUILD_DIR}/lib/include
)

add_dependencies(open_amp-static metal-static)

set(VIRT_UART_FILE
	${BASE_DIR}/../../../../Middlewares/Third_Party/OpenAMP/virtual_driver/virt_uart.c
)

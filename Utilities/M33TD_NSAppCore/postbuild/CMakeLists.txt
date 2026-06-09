cmake_minimum_required(VERSION 3.10)

# Detect standalone mode (run directly) vs. subdirectory mode
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(M33TD_NSAppCore_postbuild NONE)
    # In standalone mode, set NS_BUILD_DIR to parent dir if not set
    if(NOT DEFINED NS_BUILD_DIR)
        set(NS_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/../" CACHE PATH "Build dir")
    endif()
else()
    # In subdir mode, expect NS_BUILD_DIR to be set by parent
    if(NOT DEFINED NS_BUILD_DIR)
        set(NS_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "Build dir")
    endif()
endif()

# Required variables (should be set by parent or CLI)
set(PROJECT_NAME "${PROJECT_NAME}" CACHE STRING "Project name")
set(TFM_BUILD_DIR "${TFM_BUILD_DIR}" CACHE PATH "TFM build dir")
set(BASE_DIR "${BASE_DIR}" CACHE PATH "Base dir")

# Signing key and password logic
set(TFM_S_NS_SIGN_KEY_DEFAULT "${TFM_BUILD_DIR}/api_ns/image_signing/keys/image_s_signing_private_key.pem")
set(TFM_S_NS_SIGN_KEY "${TFM_S_NS_SIGN_KEY_DEFAULT}" CACHE PATH "Path to the Trusted Firmware signing key")
set(TFM_S_NS_SIGN_KEY_PSWD "${TFM_S_NS_SIGN_KEY_PSWD}" CACHE STRING "Password for Trusted Firmware signing key")

if(NOT EXISTS "${TFM_S_NS_SIGN_KEY}")
    message(WARNING "Signing key does not exist: ${TFM_S_NS_SIGN_KEY}. Falling back to default key.")
    set(TFM_S_NS_SIGN_KEY "${TFM_S_NS_SIGN_KEY_DEFAULT}")
endif()

set(TFM_S_NS_SIGN_KEY_PSWD_ARG "")
if(TFM_S_NS_SIGN_PSWD AND NOT TFM_S_NS_SIGN_PSWD STREQUAL "")
    set(TFM_S_NS_SIGN_KEY_PSWD_ARG --key-pswd ${TFM_S_NS_SIGN_PSWD})
endif()

find_package(Python3 REQUIRED)

# Debug echo for all options and paths
# message(STATUS "[M33TD_NSAppCore_postbuild] PROJECT_NAME: ${PROJECT_NAME}")
# message(STATUS "[M33TD_NSAppCore_postbuild] TFM_BUILD_DIR: ${TFM_BUILD_DIR}")
# message(STATUS "[M33TD_NSAppCore_postbuild] TFM_S_NS_SIGN_KEY: ${TFM_S_NS_SIGN_KEY}")
# message(STATUS "[M33TD_NSAppCore_postbuild] TFM_S_NS_SIGN_PSWD: ${TFM_S_NS_SIGN_PSWD}")
# message(STATUS "[M33TD_NSAppCore_postbuild] NS_BUILD_DIR: ${NS_BUILD_DIR}")
# message(STATUS "[M33TD_NSAppCore_postbuild] BASE_DIR: ${BASE_DIR}")
# message(STATUS "[M33TD_NSAppCore_postbuild] Python3_EXECUTABLE: ${Python3_EXECUTABLE}")

add_custom_command(
    OUTPUT ${NS_BUILD_DIR}/${PROJECT_NAME}_tfm_s_ns.bin
    COMMAND ${Python3_EXECUTABLE} ${TFM_BUILD_DIR}/api_ns/image_signing/scripts/assemble.py
        --layout ${TFM_BUILD_DIR}/api_ns/image_signing/layout_files/signing_layout_s_ns.o
        --secure ${TFM_BUILD_DIR}/api_ns/bin/tfm_s.bin
        --non_secure ${NS_BUILD_DIR}/${PROJECT_NAME}.bin
        --output ${NS_BUILD_DIR}/${PROJECT_NAME}_tfm_s_ns.bin
    WORKING_DIRECTORY ${NS_BUILD_DIR}
    COMMENT "Assembling binaries"
    VERBATIM
    DEPENDS ${NS_BUILD_DIR}/${PROJECT_NAME}.elf
)

add_custom_command(
    OUTPUT ${NS_BUILD_DIR}/${PROJECT_NAME}_tfm_s_ns_signed.bin
    COMMAND ${Python3_EXECUTABLE} ${TFM_BUILD_DIR}/api_ns/image_signing/scripts/wrapper/wrapper.py
        --version 2.1.0
        --layout ${TFM_BUILD_DIR}/api_ns/image_signing/layout_files/signing_layout_s_ns.o
        --key ${TFM_S_NS_SIGN_KEY}
        ${TFM_S_NS_SIGN_KEY_PSWD_ARG}
        --public-key-format full
        --align 1
        --pad
        --confirm
        --pad-header -H 0x800
        -s 1
        -L 128
        --measured-boot-record ${NS_BUILD_DIR}/${PROJECT_NAME}_tfm_s_ns.bin
        ${NS_BUILD_DIR}/${PROJECT_NAME}_tfm_s_ns_signed.bin
    WORKING_DIRECTORY ${TFM_BUILD_DIR}/api_ns/image_signing/scripts
    COMMENT "Signing binaries"
    VERBATIM
    DEPENDS ${NS_BUILD_DIR}/${PROJECT_NAME}_tfm_s_ns.bin
)

add_custom_command(
    OUTPUT ${BASE_DIR}/bin/tfm_s_ns_signed.bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${NS_BUILD_DIR}/${PROJECT_NAME}.bin ${BASE_DIR}/bin/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${NS_BUILD_DIR}/${PROJECT_NAME}_tfm_s_ns_signed.bin ${BASE_DIR}/bin/tfm_s_ns_signed.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${TFM_BUILD_DIR}/api_ns/bin/bl2.stm32 ${BASE_DIR}/bin/bl2.stm32
    COMMAND ${CMAKE_COMMAND} -E copy ${TFM_BUILD_DIR}/api_ns/bin/ddr_phy_signed.bin ${BASE_DIR}/bin/ddr_phy_signed.bin
    COMMENT "Copying to ./bin folder"
    VERBATIM
    DEPENDS ${NS_BUILD_DIR}/${PROJECT_NAME}_tfm_s_ns_signed.bin
)

add_custom_target(M33TD_NSAppCore_postbuild ALL
    DEPENDS ${BASE_DIR}/bin/tfm_s_ns_signed.bin
)
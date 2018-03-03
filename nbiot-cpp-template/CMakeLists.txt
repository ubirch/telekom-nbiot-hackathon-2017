# This file is mainly for use with CLion, to make sure it picks up all
# the libraries and we can use syntax highlighting.
#
# DO NOT USE RUN! Use build (M-F9) on the target "compile".

cmake_minimum_required(VERSION 3.5)
set(CMAKE_TOOLCHAIN_FILE yotta_targets/mbed-gcc/CMake/toolchain.cmake)
set(CMAKE_BUILD_TYPE Debug)

project(nbiot-cpp-template C CXX)
include(yotta_targets/calliope-mini-classic-gcc/CMake/toolchain.cmake)

add_definitions(-DNRF51 -DTARGET_NRF51 -DTARGET_NRF51_CALLIOPE)

file(GLOB MICROBIT_DAL_INC yotta_modules/microbit-dal/inc/*)
file(GLOB_RECURSE MICROBIT_DAL_SRC yotta_modules/microbit-dal/source/*.cpp)

add_library(microbit-dal ${MICROBIT_DAL_SRC})
include_directories(${MICROBIT_DAL_INC})

file(GLOB_RECURSE MICROBIT_SRC yotta_modules/microbit/source/*.cpp)

add_library(microbit ${MICROBIT_SRC})
include_directories(yotta_modules/microbit/inc)

file(GLOB MBED_INC
        yotta_modules/mbed-classic/api
        yotta_modules/mbed-classic/hal
        yotta_modules/mbed-classic/targets/cmsis
        yotta_modules/mbed-classic/targets/cmsis/TARGET_NORDIC
        yotta_modules/mbed-classic/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822
        yotta_modules/mbed-classic/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822
        yotta_modules/mbed-classic/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/TARGET_NRF51_CALLIOPE
        yotta_modules/microbit-dal/yotta_modules/mbed-drivers
        yotta_modules/microbit-dal/yotta_modules/minar
        yotta_modules/microbit-dal/yotta_modules/ble
        )
file(GLOB_RECURSE MBED_SRC
        yotta_modules/microbit-dal/yotta_modules/*.cpp
        yotta_modules/mbed-classic/common/*.c
        yotta_modules/mbed-classic/common/*.cpp
        yotta_modules/mbed-classic/targets/TARGET_NORDIC/*.c)

add_library(mbed ${MBED_SRC})
include_directories(${MBED_INC})

file(GLOB NRF51_SDK_INC
        yotta_modules/nrf51-sdk/source/nordic_sdk/components/ble
        yotta_modules/nrf51-sdk/source/nordic_sdk/components/device
        yotta_modules/nrf51-sdk/source/nordic_sdk/components/drivers_nrf
        yotta_modules/nrf51-sdk/source/nordic_sdk/components/libraries
        yotta_modules/nrf51-sdk/source/nordic_sdk/components/softdevice/common/softdevice_handler
        yotta_modules/nrf51-sdk/source/nordic_sdk/components/softdevice/s130/headers
        )
file(GLOB_RECURSE NRF51_SDK_SRC
        yotta_modules/nrf51-sdk/source/nordic_sdk/components/*.c*)

add_library(nrf51sdk ${NRF51_SDK_SRC})
include_directories(${NRF51_SDK_INC})

include(yotta_modules/ubirch-mbed-nacl-cm0/lib.cmake)

add_library(msgpack
        yotta_modules/ubirch-mbed-msgpack/msgpack/fbuffer.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/object.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/pack.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/pack_define.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/pack_template.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/sbuffer.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/sysdep.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/unpack.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/unpack_define.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/unpack_template.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/version.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/vrefbuffer.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/zbuffer.h
        yotta_modules/ubirch-mbed-msgpack/msgpack/zone.h
        yotta_modules/ubirch-mbed-msgpack/msgpack.h
        yotta_modules/ubirch-mbed-msgpack/objectc.c
        yotta_modules/ubirch-mbed-msgpack/unpack.c
        yotta_modules/ubirch-mbed-msgpack/version.c
        yotta_modules/ubirch-mbed-msgpack/vrefbuffer.c
        )
target_include_directories(msgpack PUBLIC yotta_modules/ubirch-mbed-msgpack)

add_library(ubirch-protocol
        yotta_modules/ubirch-protocol/ubirch/ubirch_protocol.h
        yotta_modules/ubirch-protocol/ubirch/ubirch_ed25519.h
        yotta_modules/ubirch-protocol/ubirch/ubirch_protocol_kex.h
        yotta_modules/ubirch-protocol/ubirch/ubirch_protocol_kex.c
        yotta_modules/ubirch-protocol/ubirch/digest/sha512.h
        yotta_modules/ubirch-protocol/ubirch/digest/sha512.c
        yotta_modules/ubirch-protocol/ubirch/digest/config.h)
target_include_directories(ubirch-protocol PUBLIC yotta_modules/ubirch-protocol)
target_link_libraries(ubirch-protocol libnacl msgpack)

add_executable(main
        source/main.cpp
        source/utils/utils.cpp
        source/utils/random.c
        # add more source files here, if needed
        )
target_link_libraries(main microbit microbit-dal ubirch-protocol nrf51sdk)

add_custom_target(compile
        COMMAND yt build
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
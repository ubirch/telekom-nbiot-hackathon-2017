# This file is mainly for use with CLion, to make sure it picks up all
# the libraries and we can use syntax highlighting.
#
# DO NOT USE RUN! Use build (M-F9) on the target "compile".

cmake_minimum_required(VERSION 3.5)
set(CMAKE_TOOLCHAIN_FILE yotta_targets/mbed-gcc/CMake/toolchain.cmake)
set(CMAKE_BUILD_TYPE Debug)

project(calliope-firmware C CXX)
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

add_executable(main
        source/main.cpp
        # add more source files here, if needed
        )
target_link_libraries(main microbit microbit-dal microbit nrf51sdk)

add_custom_target(compile
        COMMAND yt build
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

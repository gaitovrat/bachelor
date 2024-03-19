cmake_minimum_required(VERSION 3.22)

option(MCU "Build MCU targets" OFF)

if (MCU)
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR ARM)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

    set(TOOLCHAIN_PATH $ENV{TOOLCHAIN_PATH})
    if (NOT TOOLCHAIN_PATH)
        message(FATAL_ERROR "TOOLCHAIN_PATH is not set")
    else()
        message(STATUS "TOOLCHAIN_PATH: ${TOOLCHAIN_PATH}")
    endif()

    set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-c++)
    set(CMAKE_AR ${TOOLCHAIN_PATH}/arm-none-eabi-ar)
    set(CMAKE_SIZE ${TOOLCHAIN_PATH}/arm-none-eabi-size)
endif()

project(Car VERSION 0.1 LANGUAGES C CXX)

include(ProcessorCount)
ProcessorCount(NCPU)

if (${NCPU} EQUAL 0)
    set(NCPU 1)
endif()

set(CMAKE_BUILD_PARALLEL_LEVEL ${NCPU})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MCU)
    add_subdirectory(Shared)
    add_subdirectory(MCU)
else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    add_subdirectory(Externals)
    add_subdirectory(Shared)
    add_subdirectory(CarQt)
endif()

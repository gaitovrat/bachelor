cmake_minimum_required(VERSION 3.22)

project(Car VERSION 0.1 LANGUAGES C CXX)

include(ProcessorCount)
ProcessorCount(NCPU)

if (${NCPU} EQUAL 0)
    set(NCPU 1)
endif()

set(CMAKE_BUILD_PARALLEL_LEVEL ${NCPU})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Externals)
add_subdirectory(Shared)
add_subdirectory(CarQt)
add_subdirectory(Utilities)

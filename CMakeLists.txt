cmake_minimum_required(VERSION 3.25)

project(Car VERSION 0.1 LANGUAGES CXX)

include(ProcessorCount)
ProcessorCount(NCPU)

if (${NCPU} EQUAL 0)
    set(NCPU 1)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL ${NCPU})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

set(SOURCES
    Src/Image.cpp
    Src/PIDData.cpp
    Src/Region.cpp
    Src/Filter.cpp
    Src/Utils.cpp
)

add_library(Shared STATIC
    ${SOURCES}
    Include/Shared/Utils.h
    Include/Shared/Data.h
    Include/Shared/Image.h
    Include/Shared/Mode.h
    Include/Shared/PIDData.h
    Include/Shared/Region.h
    Include/Shared/Vec.h
    Include/Shared/Filter.h)

if (MCU)
    target_compile_definitions(Shared PRIVATE -D__CODE_RED -D__NEWLIB__ -DDEBUG)
    target_compile_options(Shared PRIVATE
        -O0
        -fno-common
        -g3
        -gdwarf-4
        -Wall
        -fmessage-length=0
        -fno-builtin
        -ffunction-sections
        -fdata-sections
        -fno-rtti
        -fno-exceptions
        -fmerge-constants
        -mcpu=cortex-m4
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -mthumb
        -fstack-usage
        -specs=nano.specs)
else()
    target_compile_definitions(Shared PUBLIC -DPC)
    target_link_libraries(Shared PUBLIC jsoncpp_static)
endif()

target_include_directories(Shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)

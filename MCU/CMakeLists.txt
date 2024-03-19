set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/board
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS
    ${CMAKE_CURRENT_SOURCE_DIR}/component
    ${CMAKE_CURRENT_SOURCE_DIR}/component/osa
    ${CMAKE_CURRENT_SOURCE_DIR}/component/uart
    ${CMAKE_CURRENT_SOURCE_DIR}/component/lists
    ${CMAKE_CURRENT_SOURCE_DIR}/component/serial_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/device
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip/port
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip/template
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/mdio
    ${CMAKE_CURRENT_SOURCE_DIR}/phy
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/startup
    ${CMAKE_CURRENT_SOURCE_DIR}/tfc
    ${CMAKE_CURRENT_SOURCE_DIR}/utilities
)

file(GLOB_RECURSE C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB_RECURSE CXX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME}.afx ${C_SOURCES} ${CXX_SOURCES})

target_include_directories(${PROJECT_NAME}.afx PRIVATE ${INCLUDES})
target_compile_definitions(${PROJECT_NAME}.afx PRIVATE
    -DCPU_MK66FN2M0VMD18
    -DCPU_MK66FN2M0VMD18_cm4
    -DSDK_OS_BAREMETAL
    -DSERIAL_PORT_TYPE_UART=1
    -DSDK_DEBUGCONSOLE=1
    -D__MCUXPRESSO
    -DDEBUG
    -D__NEWLIB__
)

target_compile_options(${PROJECT_NAME}.afx PRIVATE
    -O0
    -fno-common
    -g3
    -gdwarf-4
    -Wall
    -ffunction-sections
    -fdata-sections
    -ffreestanding
    -fno-builtin
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    -fno-exceptions
    -fmerge-constants
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -fstack-usage
    -specs=nano.specs
)

target_link_options(${PROJECT_NAME}.afx PRIVATE
    -nostdlib
    -L${CMAKE_CURRENT_SOURCE_DIR}/scripts
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -T ${CMAKE_CURRENT_SOURCE_DIR}/scripts/linker.ld
)

target_link_libraries(${PROJECT_NAME}.afx PRIVATE Shared)
add_custom_command(TARGET ${PROJECT_NAME}.afx POST_BUILD
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.afx
    COMMENT "Invoking: Cross ARM GNU Print Size"
)

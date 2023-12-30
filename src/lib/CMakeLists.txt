set(lib_Include ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(lib_Link CarLib PARENT_SCOPE)

add_library(CarLib
    Vec.h
    Vec.cpp)

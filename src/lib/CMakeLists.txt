set(lib_Include ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(lib_Link carlib PARENT_SCOPE)

add_library(${lib_Link} STATIC
    Vec.h
    Vec.cpp)


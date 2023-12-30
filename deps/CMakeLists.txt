include(ExternalProject)

# Setup Qt6
set(QT_BINARY_DIR ${CMAKE_BINARY_DIR}/deps/qt)
set(QT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qt)
set(Qt6_DIR ${QT_BINARY_DIR}/qtbase/lib/cmake/Qt6 PARENT_SCOPE)

if (NOT EXISTS ${QT_BINARY_DIR})
    file(MAKE_DIRECTORY ${QT_BINARY_DIR})
    execute_process(
        COMMAND ${QT_SOURCE_DIR}/configure
        WORKING_DIRECTORY ${QT_BINARY_DIR}
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${QT_BINARY_DIR} --parallel ${NCPU}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

include(ExternalProject)

set(QT_BINARY_DIR ${CMAKE_BINARY_DIR}/deps/qt)
set(QT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qt)

if(NOT EXISTS ${QT_BINARY_DIR})
    message("Create ${QT_BINARY_DIR}.")
    file(MAKE_DIRECTORY ${QT_BINARY_DIR})
endif()

ExternalProject_Add(
    qt
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qt
    BINARY_DIR ${QT_BINARY_DIR}
    CONFIGURE_COMMAND ${QT_SOURCE_DIR}/configure -prefix ${QT_BINARY_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build ${QT_BINARY_DIR} --parallel ${NCPU}
    INSTALL_COMMAND ""
)

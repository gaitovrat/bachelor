cmake_minimum_required(VERSION 3.5)

project(carui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network SerialPort)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network SerialPort)

set(PROJECT_SOURCES
        carui/main.cpp
        carui/mainwindow.cpp
        carui/mainwindow.h
        carui/mainwindow.ui
        carui/udpclient.h
        carui/udpclient.cpp
        carui/baseclient.h
        carui/baseclient.cpp
        carui/serialclient.h
        carui/serialclient.cpp
)

include_directories(common)

qt_add_executable(carui
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)
target_link_libraries(carui PRIVATE Qt6::Widgets Qt6::Network Qt6::SerialPort)

set_target_properties(carui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS carui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(carui)

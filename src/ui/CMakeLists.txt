set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_COMPONENTS
    Network
    SerialPort
    Widgets
)

find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})

add_executable(carui
    main.cpp
    Settings.cpp
    Settings.h
    window/MainWindow.cpp
    window/MainWindow.h
    window/MainWindow.ui
    window/SettingsWindow.h 
    window/SettingsWindow.cpp 
    window/SettingsWindow.ui
    client/BaseClient.cpp
    client/BaseClient.h
    client/UdpClient.h
    client/UdpClient.cpp
    client/SerialClient.cpp
    client/SerialClient.h
)

list(TRANSFORM QT_COMPONENTS PREPEND Qt6::)

target_include_directories(carui PRIVATE 
    ${lib_Include} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${jsoncpp_SOURCE_DIR}/include)
target_link_libraries(carui
    PRIVATE 
    ${lib_Link}
    ${QT_COMPONENTS}
    jsoncpp_lib)

set_target_properties(carui PROPERTIES
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

find_package(Qt6 6.4 REQUIRED COMPONENTS Network SerialPort Quick)

qt_policy(SET QTP0001 NEW)
qt_standard_project_setup()

qt_add_executable(carui
    main.cpp
    App.cpp
    App.h
    client/BaseClient.cpp
    client/BaseClient.h
    client/SerialClient.cpp
    client/SerialClient.h
    client/UdpClient.cpp
    client/UdpClient.h)
qt_add_qml_module(carui
    URI carui
    VERSION 1.0
    QML_FILES assets/main.qml
)

target_include_directories(carui PRIVATE ${lib_Include})
target_link_libraries(carui 
    PRIVATE 
    ${lib_Link}
    Qt6::Network Qt6::SerialPort Qt6::Quick)
set_target_properties(carui PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

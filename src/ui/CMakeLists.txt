find_package(Qt6 REQUIRED COMPONENTS Network SerialPort)

qt_standard_project_setup()

qt_add_executable(carui
    main.cpp
    client/baseclient.cpp
    client/baseclient.h
    client/serialclient.cpp
    client/serialclient.h
    client/udpclient.cpp
    client/udpclient.h)

target_link_libraries(carui PRIVATE Qt6::Network Qt6::SerialPort)
set_target_properties(carui PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

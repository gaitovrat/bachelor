set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_COMPONENTS
    Network
    SerialPort
    Widgets
)

find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})

add_executable(carqt
    Src/main.cpp
    Src/Settings.cpp
    Src/Settings.h
    Src/Window/MainWindow.cpp
    Src/Window/MainWindow.h
    Src/Window/MainWindow.ui
    Src/Window/SettingsWindow.h
    Src/Window/SettingsWindow.cpp
    Src/Window/SettingsWindow.ui
    Src/Client/BaseClient.cpp
    Src/Client/BaseClient.h
    Src/Client/UdpClient.h
    Src/Client/UdpClient.cpp
    Src/Client/SerialClient.cpp
    Src/Client/SerialClient.h
    Src/Utils.h
    Src/Utils.cpp
    Resources/resources.qrc
)

list(TRANSFORM QT_COMPONENTS PREPEND Qt6::)

target_include_directories(carqt PRIVATE
    ${Shared_Include}
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${jsoncpp_SOURCE_DIR}/include)
target_link_libraries(carqt
    PRIVATE 
    ${QT_COMPONENTS}
    jsoncpp_static)

set_target_properties(carqt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER carqt.gaitov
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(carqt)

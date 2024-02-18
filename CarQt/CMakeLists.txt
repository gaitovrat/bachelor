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
    src/main.cpp
    src/Settings.cpp
    src/Settings.h
    src/window/MainWindow.cpp
    src/window/MainWindow.h
    src/window/MainWindow.ui
    src/window/SettingsWindow.h
    src/window/SettingsWindow.cpp
    src/window/SettingsWindow.ui
    src/client/BaseClient.cpp
    src/client/BaseClient.h
    src/client/UdpClient.h
    src/client/UdpClient.cpp
    src/client/SerialClient.cpp
    src/client/SerialClient.h
    src/Utils.h
    src/Utils.cpp
    resources/resources.qrc
)

list(TRANSFORM QT_COMPONENTS PREPEND Qt6::)

target_include_directories(carqt PRIVATE
    ${shared_include}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${jsoncpp_SOURCE_DIR}/include)
target_link_libraries(carqt
    PRIVATE 
    ${QT_COMPONENTS}
    jsoncpp_static)

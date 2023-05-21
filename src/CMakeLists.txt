cmake_minimum_required(VERSION 3.16)

project(Blueline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Quick REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    UserInterface.cpp
    BluetoothManager.cpp
    DeviceManager.cpp
    AudioIOManager.cpp
    qml.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick)

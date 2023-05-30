cmake_minimum_required(VERSION 3.16)

project(Blueline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  Qt6
  COMPONENTS Bluetooth Multimedia Core Quick
  REQUIRED)

add_executable(
  ${PROJECT_NAME}
  main.cpp
  UserInterface.cpp
  UserInterface.h
  BluetoothManager.cpp
  BluetoothManager.h
  Device.cpp
  Device.h
  DeviceManager.cpp
  DeviceManager.h
  AudioIOManager.cpp
  AudioIOManager.h
  qml.qrc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Bluetooth Qt6::Multimedia
                                              Qt6::Core Qt6::Quick)

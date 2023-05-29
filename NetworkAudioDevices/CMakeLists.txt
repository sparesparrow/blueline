cmake_minimum_required(VERSION 3.16)

project(AudioStreamingApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  Qt6
  COMPONENTS Core Multimedia Network
  REQUIRED)

add_executable(
  ${PROJECT_NAME}
  main.cpp
  AudioService.h
  AudioPlayer.h
  AudioPlayer.cpp
  AudioStreamer.h
  AudioStreamer.cpp
  InterfaceSocket.h
  InterfaceSocketServer.h
  InterfaceSocketServer.cpp
  InterfaceSocketClient.h
  InterfaceSocketClient.cpp
  InterfaceSocketFactory.h
  NetworkPeer.h
  NetworkPeer.cpp
  NetworkManager.h
  NetworkManager.cpp
  MainWindow.h
  MainWindow.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Multimedia
                                              Qt6::Network)

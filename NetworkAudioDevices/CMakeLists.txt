cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "BluelineAudio")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Multimedia Network Gui Widgets Quick)

add_executable(
  ${PROJECT_NAME}
  main.cpp
  AudioService.h
<<<<<<< HEAD
=======
  AudioServiceFactory.h
  # AudioService.cpp
  # AudioServiceFactory.cpp
>>>>>>> 3ce8ad6 (AudioService redesign and factories)
  NetworkPeer.h
  BroadcastDiscoveryService.h
  # NetworkPeer.cpp
  NetworkManager.h
  NetworkManager.cpp
  MainWindow.h
  MainWindow.cpp)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Multimedia Qt6::Network Qt6::Gui
                          Qt6::Widgets Qt6::Quick)

install(TARGETS BluelineAudio # RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
        # BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
        # LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

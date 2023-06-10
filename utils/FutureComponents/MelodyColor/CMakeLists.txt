
cmake_minimum_required(VERSION 3.19)

set(PROJECT_NAME "MelodyColor")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Multimedia Gui Widgets Quick)

add_executable(
  ${PROJECT_NAME}
    src/main.cpp
    src/SpectrumAnalyser.h
    src/SpectrumAnalyser.cpp
    qml/qml.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Multimedia Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Quick)

install(TARGETS ${PROJECT_NAME})

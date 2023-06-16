cmake_minimum_required(VERSION 3.19)

set(PROJECT_NAME "MelodyColor")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(KISSFFT_DATATYPE "int16_t")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Multimedia Widgets)

# Add the path to the kissfft directory
add_subdirectory(src/3rdParty/kissfft)

add_executable(
  ${PROJECT_NAME}
  src/main.cpp
  src/AudioCapture.h
  src/AudioProcessor.h
  src/AudioService.h
  src/SpectrumVisualizer.h
  src/AudioPlayer.h
  src/AudioServiceFactory.h
  src/AudioStreamer.h)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Quick Qt6::Multimedia
                          Qt6::Widgets kissfft::kissfft)

# Copy QML files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/qml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add the resource file to the executable
qt6_add_resources(${PROJECT_NAME} "qml" PREFIX "/" FILES "qml/main.qml")

# Add the path to the qml directory
set(QML_IMPORT_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/qml"
    CACHE STRING "Qt Creator extra qml import paths")

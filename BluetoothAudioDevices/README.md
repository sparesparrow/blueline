# Blueline 

Blueline is an advanced audio device manager designed for managing, controlling, and interfacing with your Ipv4, Bluetooth and Audio devices. 

**This part of the project is still just a concept.** It is being implemented to provide a comprehensive and streamlined system for connecting to and managing your Bluetooth audio devices, with a focus on being lightweight, easy to use, and efficient.

Features
```
    Discover Bluetooth devices in range.
    Pair and connect to Bluetooth devices.
    Control audio input and output devices.
    Send and receive data to/from Bluetooth devices.
    Ability to change input and output devices dynamically.
    Handles device states (Connected, In Use, Disconnected).
```
## Dependencies

The project depends on several Qt modules, including:

```
    QtCore
    QtBluetooth
    QtMultimedia
```
The code is written in C++ and Qt, it should be platform-independent and is confirmed to work on Windows, Linux, and MacOS.
## Building

### Requirements

```
    A compiler supporting C++17
    Qt 5.15 or later
```

### Build Instructions

```
    Open the project file Blueline.pro in Qt Creator.
    Configure the project for your specific system setup.
    Build the project by clicking on the build button.
```

## Usage

After building the project, you can run the executable that was generated. The GUI should be intuitive to use. Use the Discover Devices button to start discovering Bluetooth devices. Once discovered, you can choose to connect or pair with a device.

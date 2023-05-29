# Blueline Audio Streamer

The project aims to develop an audio streaming application using Qt6 framework. The application will support two modes: client and server. In server mode, the application will handle the audio source, which can be a local audio file or the default output audio device. The server mode will also handle Real-Time Protocol (RTP) and Synchronization Source (SSRC) for any number of clients, providing synchronized audio for all of them. In client mode, the application will connect to the server on the local network and receive the audio stream for playback.

## Project Overview

The Blueline project consists of the following components:

- NetworkManager: Manages the network communication and discovery between the server and client devices.
- AudioStreamer: Handles audio streaming from the server device to the clients.
- AudioPlayer: Receives and plays the streamed audio on the client devices.
- MainWindow: Provides a user interface to interact with the application, including displaying discovered peers and controlling the audio streaming process.

## Features

- Network Discovery: The application automatically discovers available server devices on the local network.
- Server Mode: The server device handles the audio source, which can be a local audio file or the capture of the default output audio device.
- Client Mode: The client devices connect to the server device and receive synchronized audio playback.
- Start/Stop Streaming: Allows the server device to start and stop audio streaming to the clients.
- Start/Stop Playback: Enables the clients to start and stop audio playback received from the server.
- User Interface: Provides a user-friendly interface to display discovered peers, control connections, and audio streaming/playback.

## Software Design

The software design for this project involves multiple classes and their interactions to achieve the desired functionality. Here is an overview of the main classes and their responsibilities:

- AudioStreamer: Handles audio streaming in both the server and client modes.
- AudioPlayer: Plays audio data received from the server in the client mode.
- ConnectionManager: Manages network-related functionality, including instance discovery and network sockets.
- NetworkService: Represents a network interface and its associated UDP socket.
- BroadcastDiscoveryService: Implements the broadcast mechanism for discovering instances on the local network.
- User Interfaces: Provide interfaces for the server and client modes to control streaming and display discovered peers.

## Getting Started

To build and run the project, follow these steps:

1. Ensure you have the required dependencies:
   - Qt6 framework with the necessary modules: `Bluetooth, Multimedia, Core, Quick, Network`.

2. Build the project:
   - Use CMake to generate the build files.
   - Configure the project with the required Qt6 libraries.
   - Build the project using the generated build files.

3. Run the application:
   - Launch the built executable.
   - The main window will appear, displaying the discovered instances (in client mode) or the server controls (in server mode).
   - Interact with the user interface to control the streaming functionality or connect to a server for audio playback.

## Future Enhancements

- Implement error handling and robustness features, such as handling network disconnections, resuming playback after interruptions, and handling audio format compatibility between server and clients.
- Improve the user interfaces to provide a more intuitive and user-friendly experience. Add features like progress bars, volume controls, and visual indicators for streaming status.
- Enhance the network discovery mechanism by implementing more advanced techniques like mDNS (Multicast DNS) or integrating with existing service discovery protocols.
- Add security measures to the network communication, such as encryption and authentication, to ensure secure audio streaming.
- Implement additional audio processing features, such as audio effects, equalizers, or configurable audio settings.

## Contributing

Contributions to the Blueline Audio Streamer project are welcome! If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

## License

Blueline Audio Streamer is released under the [MIT License](LICENSE).

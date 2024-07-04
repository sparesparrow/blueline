# Blueline Audio Streamer

Blueline Audio Streamer is an application developed using the Qt6 framework, aiming to deliver an audio streaming functionality with two operating modes: server and client. In server mode, it handles the audio source, which could either be a local audio file or the default output audio device. The server is also responsible for handling the Real-Time Protocol (RTP) and Synchronization Source (SSRC) for all clients, ensuring synchronized audio across all connected devices. The client mode, on the other hand, is tasked with connecting to the server within the local network and receiving the audio stream for playback.

## Project Components

- **NetworkManager**: Responsible for managing the network communication and discovery between server and client devices.
- **AudioStreamer**: Handles the streaming of audio from the server device to the clients.
- **AudioPlayer**: Tasked with receiving and playing the streamed audio on client devices.
- **MainWindow**: The user interface that provides interactive functionality to control the application.

## Features

- Network Discovery: Discover available server devices on the local network automatically.
- Server Mode: Handle the audio source, be it a local audio file or a captured default output audio device.
- Client Mode: Connect to the server device and receive synchronized audio playback.
- Start/Stop Streaming: Control the server's audio streaming to clients.
- Start/Stop Playback: Control the client's audio playback received from the server.
- User Interface: An easy-to-use interface to display discovered peers, control connections, and manage audio streaming.

## Software Design

The software design involves interactions between several classes to fulfill the desired functionality:

- **AudioStreamer**: Handles audio streaming for both server and client modes.
- **AudioPlayer**: Takes care of playing audio data received from the server in client mode.
- **ConnectionManager**: Manages network-related functionality, such as discovery and network sockets.
- **NetworkService**: Represents a network interface with its associated UDP socket.
- **BroadcastDiscoveryService**: Implements the broadcast mechanism for discovering instances on the local network.
- **User Interfaces**: Provides interfaces for both the server and client modes to control streaming and display discovered peers.

## Getting Started

Follow these steps to build and run the project:

1. Ensure you have the required dependencies:
    - Qt6 framework with the necessary modules: `Core, Quick, Network, Multimedia`.

2. Build the project:
    - Use CMake to generate the build files.
    - Configure the project with the necessary Qt6 libraries.
    - Build the project using the generated build files.

3. Run the application:
    - Launch the built executable.
    - The main window will appear, displaying the discovered instances (in client mode) or the server controls (in server mode).
    - Interact with the user interface to control the streaming functionality or connect to a server for audio playback.

## Future Implementations

- Implement error handling to manage network disconnections and resume playback after interruptions.
- Enhance user interfaces for better user experience. Add features like progress bars, volume controls, and visual indicators for streaming status.
- Improve the network discovery mechanism by using advanced techniques or integrating existing service discovery protocols.
- Implement security measures to secure audio streaming, like encryption and authentication.
- Additional audio processing features, such as audio effects, equalizers, and configurable audio settings.
- Bluetooth device discovery and pairing.
- Control of audio input and output devices.
- Data transfer to/from Bluetooth devices.
- Dynamic switching of input and output devices.
- Handle device states (Connected, In Use, Disconnected).
- Implement the multicast feature for sending audio data to a multicast group.
- Support various synchronization protocols like SSRC, NTP, PTP, RTSP, SDP, SRTP, or WebRTC.

## Contributing

Contributions to the Blueline Audio Streamer project are welcome! If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

## License

Blueline Audio Streamer is licensed under the [GNU General Public License v3.0](./LICENSE). You are free to use, modify, and distribute the software as long as you follow the terms of the license. For more information, visit https://www.gnu.org.


# 'Pull requests' from [Hard-Coder](https://chatgpt.com/share/351caa1f-f91f-4cb5-9418-380aa29c8d51):

- [**update.md**](update.md)
- [**update1.md**](update1.md)
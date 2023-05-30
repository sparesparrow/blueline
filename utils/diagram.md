     +------------------+
     |    MainWindow    |
     +------------------+
     | - peerListWidget : QListWidget*
     | - startDiscoveryButton : QPushButton*
     | - stopDiscoveryButton : QPushButton*
     | - connectButton : QPushButton*
     | - disconnectButton : QPushButton*
     | - startStreamingButton : QPushButton*
     | - stopStreamingButton : QPushButton*
     | - networkManager : NetworkManager*
     +------------------+
     | + startDiscovery() : void
     | + stopDiscovery() : void
     | + connectToPeer() : void
     | + disconnectFromPeer() : void
     | + startAudioStreaming() : void
     | + stopAudioStreaming() : void
     | + handlePeerDiscovered(name: QString, address: QString) : void
     | + handleConnectionStatusUpdated(index: int, connected: bool) : void
     +------------------+

     +------------------+
     |  NetworkManager  |
     +------------------+
     | - peers : QVector<NetworkPeer*>
     +------------------+
     | + startDiscovery() : void
     | + stopDiscovery() : void
     | + connectToPeer(address: QString) : void
     | + disconnectFromPeer(address: QString) : void
     | + startAudioStreaming(address: QString) : void
     | + stopAudioStreaming(address: QString) : void
     | + handlePeerDiscovered(name: QString, address: QString) : void
     | + handleConnectionStatusUpdated(index: int, connected: bool) : void
     +------------------+

     +------------------+
     |   NetworkPeer    |
     +------------------+
     | - socket : InterfaceSocket*
     | - audioService : AudioService*
     +------------------+
     | + handleDataReceived(data: QByteArray, sender: QHostAddress, senderPort: quint16) : void
     +------------------+

     +------------------+
     |   AudioService   |
     +------------------+
     | - ssrcIdentifier : qint32
     +------------------+
     | + setSSRCIdentifier(ssrcIdentifier: qint32) : void
     | + receiveAudioData(audioData: QByteArray) : void
     +------------------+

     +------------------+
     |   AudioPlayer    |
     +------------------+
     | - audioOutput : QAudioOutput*
     +------------------+
     | + receiveAudioData(audioData: QByteArray) : void
     +------------------+

     +------------------+
     |   AudioStreamer  |
     +------------------+
     | + receiveAudioData(audioData: QByteArray) : void
     +------------------+

     +------------------+
     | InterfaceSocket  |
     +------------------+
     | - interfaceAddress : QHostAddress
     | - interfaceName : QString
     | - socket : QUdpSocket
     +------------------+
     | + startListening(groupAddress: QHostAddress, port: quint16) : void
     | + handleReadyRead() : void
     +------------------+

     +------------------+
     |InterfaceSocketServer|
     +------------------+
     | + startListening(groupAddress: QHostAddress, port: quint16) : void
     +------------------+

     +------------------+
     |InterfaceSocketClient|
     +------------------+
     | + startListening(groupAddress: QHostAddress, port: quint16) : void
     +------------------+

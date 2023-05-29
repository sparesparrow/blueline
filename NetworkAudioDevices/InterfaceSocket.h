#ifndef INTERFACESOCKET_H
#define INTERFACESOCKET_H
#include <QObject>
#include <QSharedPointer>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>


class NetConfFactory : public QObject
{

    Q_OBJECT

public:
    // Constructor 
    explicit NetConfFactory(QObject *parent = nullptr) : QObject(parent)
    {

        for (const auto& interface : QNetworkInterface::allInterfaces())
        {
            for (const auto& entry : interface.addressEntries()) {
                interfaceAddresses[interface.name()].push_back(entry.ip());
            }
        }
        QUdpSocket* udpSocket = new QUdpSocket(this);
        socket = QSharedPointer<QUdpSocket>(udpSocket);
        QNetworkInterface::allInterfaces():

        if (interface.flags().testFlag(QNetworkInterface::IsUp) && !addresses.empty())
        {
            udpSocket = QSharedPointer<QUdpSocket>(new QUdpSocket(this));
        }

        socket.bind(QHostAddress::AnyIPv4, 0);
        connect(&socket, &QUdpSocket::readyRead, this, &NetConfFactory::onDataReceived);
    }

    virtual ~NetConfFactory() = default;

    QString getInterfaceName() const { return interfaceName; }
    QSharedPointer<QUdpSocket> getUdpSocket() const { return udpSocket; }
    int getPort() const { return serverPort; }
    QList<QHostAddress> getAddresses() const { return addresses; }
    QHostAddress getAddress(int index = 0) const { return addresses[index]; }
    bool isUdpSocketInitialized() const { return (udpSocket != nullptr); }
    bool isLoopbackInterface() const { return (interfaceName == "lo"); }

    QSharedPointer<QIODevice> createSocket(const QNetworkInterface& interface)
    {

        if (interface.flags().testFlag(QNetworkInterface::IsUp) && !addresses.empty())
        {

            if (udpSocket->bind(interface.name(), serverPort)) {
                qDebug() << "Server started listening on port " << serverPort << " for all interfaces.";
                connect(&*udpSocket, &QUdpSocket::readyRead, this, &NetworkService::processPendingDatagrams);
            } else {
                qDebug() << "Failed to bind server socket on port " << serverPort << ".";
            }
            interfaceAddresses[interface.name()] = QSharedPointer<NetworkService>(new NetworkService(interface, port));
        }
        return QSharedPointer<BroadcastDiscoveryService>(new BroadcastDiscoveryService(discoveryPort));
    }

    QSharedPointer<BroadcastDiscoveryService> createBroadcastDiscovery()
    {
        return QSharedPointer<BroadcastDiscoveryService>(new BroadcastDiscoveryService(discoveryPort));
    }

private:
    QMap<QString, QList<QHostAddress> > interfaceAddresses;
    static constexpr const int audioProviderPort = 3101;
    static constexpr const int audioPlayerPort = 3102;
    static constexpr const int peerDiscoveryPort = 3103;
    QMap<QSharedPointer<NetworkPeer>, QList<QSharedPointer<NetworkPeer> > networkPeers;
};


class NetworkPeer : public QObject
{

    Q_OBJECT

public:
    // Constructor 
    explicit NetworkPeer(QObject *parent = nullptr) : QObject(parent)
    {
        QUdpSocket* udpSocket = new QUdpSocket(this);
        socket = QSharedPointer<QUdpSocket>(udpSocket);
        socket.bind(QHostAddress::AnyIPv4, 0);
        connect(&socket, &QUdpSocket::readyRead, this, &NetworkPeer::onDataReceived);
    }

    virtual ~NetworkPeer() = default;
    // Method to send data to a specific host and port.
    void writeDatagram(const QByteArray &data, const QHostAddress &host, quint16 port)
    {
        socket.writeDatagram(data, host, port);
    }


    QString getInterfaceName() const { return interfaceName; }
    QSharedPointer<QUdpSocket> getUdpSocket() const { return udpSocket; }
    int getPort() const { return serverPort; }
    QList<QHostAddress> getAddresses() const { return addresses; }
    QHostAddress getAddress(int index = 0) const { return addresses[index]; }
    bool isUdpSocketInitialized() const { return (udpSocket != nullptr); }
    bool isLoopbackInterface() const { return (interfaceName == "lo"); }
signals:
    void dataReceived(QByteArray data, QHostAddress senderAddress, quint16 senderPort);

private slots:
    void onDataReceived()
    {
        while (socket.hasPendingDatagrams())
        {
            QNetworkDatagram datagram = socket->receiveDatagram();
            QByteArray message = datagram.data();
            QHostAddress senderAddress = datagram.senderAddress();
            quint16 senderPort = datagram.senderPort();
            
            datagram.resize(int(socket.pendingDatagramSize()));
            socket.readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

            emit dataReceived(datagram, senderAddress, senderPort);
        }
    }
    void processPendingDatagrams()
    {
        while (socket->hasPendingDatagrams())
        {
            QNetworkDatagram datagram = socket->receiveDatagram();
            QByteArray message = datagram.data();
            QHostAddress senderAddress = datagram.senderAddress();
            quint16 senderPort = datagram.senderPort();
            qDebug() << "Received datagram | " << senderAddress.toString() << ":" << senderPort << " | "<< message;

            // TODO: - Handle synchronization and share AudioStreamer and AudioPlayer over network
            //       - Define message types in enum class 
            if (message == "RTP")
            {
                if (senderPort == serverPort) {
                    // Use existing UDP socket for RTP communication as server
                    qDebug() << "message == RTP && senderPort == serverPort";
                    socket->connectToHost(senderAddress, senderPort);
                }
                else {
                    qDebug() << "message == RTP && senderPort != serverPort";
                    // Create new UDP socket for RTP communication as client
                    QSharedPointer<QUdpSocket> streamSocket(new QUdpSocket(this));
                    streamSocket->bind(getAddress(), senderPort);
                    streamSocket->connectToHost(senderAddress, senderPort);
                }
            }
            else if (message == "SSRC")
            {
                // TODO: Implement SSRC and other requests/responses
            }
        }
    }
private:
    QSharedPointer<QIODevice> socket;
    QMap<QString, QSharedPointer<NetworkService>> interfaceSockets;
    QSharedPointer<BroadcastDiscoveryService> broadcastDiscoveryService;
    static constexpr const int serverPort = 3101;
    static constexpr const int clientPort = 3102;
    static constexpr const int discoveryPort = 3103;
    QList<QSharedPointer<NetworkPeer> > connectedClients;
};

#endif // INTERFACESOCKET_H
/*

class NetworkService : public NetworkService
{
    Q_OBJECT
public:
    explicit NetworkService(const QNetworkInterface& interface, const int& port, QObject* parent = nullptr)
        : NetworkService(parent), interfaceName(interface.name()), serverPort(port)
    {
        for (const auto& entry : interface.addressEntries()) {
            addresses.push_back(entry.ip());
        }
        if (interface.flags().testFlag(QNetworkInterface::IsUp) && !addresses.empty())
        {
            udpSocket = QSharedPointer<QUdpSocket>(new QUdpSocket(this));
            if (udpSocket->bind(QHostAddress::AnyIPv4, serverPort)){
                qDebug() << "Server started listening on port " << serverPort << " for all interfaces.";
                connect(&*udpSocket, &QUdpSocket::readyRead, this, &NetworkService::processPendingDatagrams);
            } else {
                qDebug() << "Failed to bind server socket on port " << serverPort << ".";
            }
        }
    }

    QString getInterfaceName() const { return interfaceName; }
    QSharedPointer<QUdpSocket> getUdpSocket() const { return udpSocket; }
    int getPort() const { return serverPort; }
    QList<QHostAddress> getAddresses() const { return addresses; }
    QHostAddress getAddress(int index = 0) const { return addresses[index]; }
    bool isUdpSocketInitialized() const { return (udpSocket != nullptr); }
    bool isLoopbackInterface() const { return (interfaceName == "lo"); }

private slots:
    void processPendingDatagrams()
    {
        while (udpSocket->hasPendingDatagrams())
        {
            QNetworkDatagram datagram = udpSocket->receiveDatagram();
            QByteArray message = datagram.data();
            QHostAddress senderAddress = datagram.senderAddress();
            quint16 senderPort = datagram.senderPort();
            qDebug() << "Received datagram | " << senderAddress.toString() << ":" << senderPort << " | "<< message;

            // TODO: - Handle synchronization and share AudioStreamer and AudioPlayer over network
            //       - Define message types in enum class 
            if (message == "RTP")
            {
                if (senderPort == serverPort) {
                    // Use existing UDP socket for RTP communication as server
                    qDebug() << "message == RTP && senderPort == serverPort";
                    udpSocket->connectToHost(senderAddress, senderPort);
                }
                else {
                    qDebug() << "message == RTP && senderPort != serverPort";
                    // Create new UDP socket for RTP communication as client
                    QSharedPointer<QUdpSocket> streamSocket(new QUdpSocket(this));
                    streamSocket->bind(getAddress(), senderPort);
                    streamSocket->connectToHost(senderAddress, senderPort);
                }
            }
            else if (message == "SSRC")
            {
                // TODO: Implement SSRC and other requests/responses
            }
        }
    }

private:
    QString interfaceName;
    QSharedPointer<QUdpSocket> udpSocket;
    QList<QHostAddress> addresses;
    int serverPort;
};
*/
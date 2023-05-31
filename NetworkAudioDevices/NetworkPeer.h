// InterfaceSocket.h
#include <QObject>
#include <QSharedPointer>
#include "AudioService.h"

class InterfaceSocket : public QObject {
    Q_OBJECT

public:
    explicit InterfaceSocket(const QNetworkInterface& networkInterface, QObject* parent = nullptr) 
        : QObject(parent) 
        , interfaceAddress(networkInterface.addressEntries().first().ip())
        , interfaceName(networkInterface.humanReadableName())
    {
        connect(&socket, &QUdpSocket::readyRead, this, &InterfaceSocket::onReadyRead);
    }

    virtual ~InterfaceSocket() {}

    virtual void startListening(const QHostAddress &groupAddress, quint16 port) = 0;

signals:
    void dataReceived(QByteArray data, QHostAddress sender, quint16 senderPort);

private slots:
    void onReadyRead() {
        while (socket.hasPendingDatagrams()) {
            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            
            data.resize(int(socket.pendingDatagramSize()));
            socket.readDatagram(data.data(), data.size(), &sender, &senderPort);
            
            emit dataReceived(data, sender, senderPort);
        }
    }

protected:
    QHostAddress interfaceAddress;
    QString interfaceName;
    QUdpSocket socket;
};

// InterfaceSocketServer.h
class InterfaceSocketServer : public InterfaceSocket {
public:
    explicit InterfaceSocketServer(const QNetworkInterface& networkInterface, QObject* parent = nullptr)
        : InterfaceSocket(networkInterface, parent) {}

    void startListening(const QHostAddress& groupAddress, quint16 port) override {
        socket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        socket.joinMulticastGroup(groupAddress);
    }
};

// InterfaceSocketClient.h
class InterfaceSocketClient : public InterfaceSocket {
public:
    explicit InterfaceSocketClient(const QNetworkInterface& networkInterface, QObject* parent = nullptr)
        : InterfaceSocket(networkInterface, parent) {}

    void startListening(const QHostAddress& groupAddress, quint16 port) override {
        socket.bind(interfaceAddress, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        socket.joinMulticastGroup(groupAddress);
    }
};

// InterfaceSocketFactory.h
class InterfaceSocketFactory {
public:
    InterfaceSocketFactory(QNetworkInterface networkInterface)
        : interfaceAddress(networkInterface.addressEntries().first().ip())
        , interfaceName(networkInterface.humanReadableName()) {}

    QSharedPointer<InterfaceSocket> createInterfaceSocket(bool server) {
        if (server)
            return QSharedPointer<InterfaceSocket>(new InterfaceSocketServer(interfaceName));
        else
            return QSharedPointer<InterfaceSocket>(new InterfaceSocketClient(interfaceName));
    }

private:
    QHostAddress interfaceAddress;
    QString interfaceName;
};

// NetworkPeer.h
class NetworkPeer : public QObject {
    Q_OBJECT

public:
    NetworkPeer(QSharedPointer<InterfaceSocket> socket, QObject* parent = nullptr)
        : QObject(parent)
        , socket(socket)
    {
        if (auto* serverSocket = dynamic_cast<InterfaceSocketServer*>(socket.get())) {
            audioService = new AudioStreamer(this);
        } else {
            audioService = new AudioPlayer(this);
        }

        connect(socket.get(), &InterfaceSocket::dataReceived, audioService, &AudioService::receiveAudioData);
    }

private:
    QSharedPointer<InterfaceSocket> socket;
    AudioService* audioService;
};

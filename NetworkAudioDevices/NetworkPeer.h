// NetworkPeer.h
#ifndef NETWORKPEER_H
#define NETWORKPEER_H
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

    void startListening(const QHostAddress &groupAddress, quint16 port) {
        socket.bind(interfaceAddress, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        socket.joinMulticastGroup(groupAddress);
    }

signals:
    void dataReceived(QSharedPointer<QUdpSocket> socket);
    void dataBytesReceived(QSharedPointer<QByteArray> data, QHostAddress sender, quint16 senderPort);
    void dataBufferReceived(QSharedPointer<QByteArray> data);

private slots:
    void onReadyRead() {
        while (socket.hasPendingDatagrams()) {
            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            
            data.resize(int(socket.pendingDatagramSize()));
            socket.readDatagram(data.data(), data.size(), &sender, &senderPort);

            qDebug("InterfaceSocket::onReadyRead data:|%s| size:|%lli|", data.data(), data.size());

            emit dataReceived(QSharedPointer<QUdpSocket>(&socket));
            emit dataBytesReceived(QSharedPointer<QByteArray>(&data), sender, senderPort);
            emit dataBufferReceived(QSharedPointer<QByteArray>(&data));
        }
    }

protected:
    QHostAddress interfaceAddress;
    QString interfaceName;
    QUdpSocket socket;
};

class InterfaceSocketFactory {
public:
    InterfaceSocketFactory(QString interfaceName): interfaceName(interfaceName)
    {}
    QSharedPointer<InterfaceSocket> create() {
        return QSharedPointer<InterfaceSocket>(new InterfaceSocket(QNetworkInterface::interfaceFromName(interfaceName)));
    }
private:
    QString interfaceName;
};

class NetworkPeer : public QObject {
    Q_OBJECT

public:
    NetworkPeer(QSharedPointer<InterfaceSocket> socket, QObject* parent = nullptr)
        : QObject(parent)
        , socket(socket)
    {}
protected:
    QSharedPointer<InterfaceSocket> socket;
};

class NetworkPeerAudioService : public NetworkPeer {
public:
    NetworkPeerAudioService(QSharedPointer<InterfaceSocket> socket, std::unique_ptr<AudioService> audioService, QObject* parent = nullptr)
        : NetworkPeer(socket, parent)
        , audioService(std::move(audioService))
    {
        connect(this->socket.get(), &InterfaceSocket::dataReceived, this->audioService.get(), &AudioService::audioDataReceived);
        connect(this->socket.get(), &InterfaceSocket::dataBufferReceived, this->audioService.get(), &AudioService::audioBufferReceived);
    }
private:
    std::unique_ptr<AudioService> audioService;
};

#endif // NETWORKPEER_H

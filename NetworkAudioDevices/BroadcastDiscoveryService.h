// BroadcastDiscoveryService.h
#ifndef BROADCASTDISCOVERYSERVICE_H
#define BROADCASTDISCOVERYSERVICE_H

#include <QObject>
#include <QUdpSocket>

// BroadcastDiscoveryService is responsible for network discovery.
class BroadcastDiscoveryService : public QObject
{
    Q_OBJECT

public:
    explicit BroadcastDiscoveryService(QObject *parent = nullptr) : QObject(parent)
    {
        socket.bind(QHostAddress::AnyIPv4, 0);
        connect(&socket, &QUdpSocket::readyRead, this, &BroadcastDiscoveryService::onDataReceived);
    }

signals:
    // Signal to emit when a server is found.
    void foundServer(QHostAddress server, quint16 serverPort);

private slots:
    void onDataReceived()
    {
        while (socket.hasPendingDatagrams()) {
            QByteArray datagram;
            QHostAddress sender;
            quint16 senderPort;

            datagram.resize(int(socket.pendingDatagramSize()));
            socket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

            // Check if the datagram is a server broadcast.
            if (/* TODO: Implement check for server broadcast */) {
                emit foundServer(sender, senderPort);
            }
        }
    }

private:
    QUdpSocket socket;
};

#endif // BROADCASTDISCOVERYSERVICE_H
/*


class BroadcastDiscoveryService : public NetworkService
{
    Q_OBJECT
public:
    explicit BroadcastDiscoveryService(int discoveryPort, QObject* parent = nullptr)
        : NetworkService(parent), discoveryPort(discoveryPort)
    {
        udpSocket = QSharedPointer<QUdpSocket>(new QUdpSocket(this));
        if (udpSocket->bind(QHostAddress::AnyIPv4, discoveryPort)) {
            qDebug() << "Instance Discovery broadcast socket bound on all interfaces and port " << discoveryPort;
            connect(&*udpSocket, &QUdpSocket::readyRead, this, &BroadcastDiscoveryService::receiveDiscoveryMessage);
        } else {
            qDebug() << "Failed to bind broadcast discovery socket on all interfaces and port " << discoveryPort;
        }
        broadcastTimer = QSharedPointer<QTimer>(new QTimer(this));
        connect(&*broadcastTimer, &QTimer::timeout, this, &BroadcastDiscoveryService::sendBroadcastMessage);
    }

    QSharedPointer<QTimer> getBroadcastTimer() const { return broadcastTimer; }
    QSharedPointer<QUdpSocket> getUdpSocket() const { return udpSocket; }
    int getPort() const { return discoveryPort; }

signals:
    void instanceDiscovered(const QHostAddress& senderAddress, quint16 senderPort);

private slots:
    void sendBroadcastMessage()
    {
        qDebug() << "BroadcastDiscoveryService::sendBroadcastMessage " << discoveryMessage;
        udpSocket->writeDatagram(discoveryMessage, QHostAddress::Broadcast, discoveryPort);
    }

    void receiveDiscoveryMessage()
    {
        while (udpSocket->hasPendingDatagrams())
        {
            QNetworkDatagram datagram = udpSocket->receiveDatagram();
            QByteArray message = datagram.data();
            QHostAddress senderAddress = datagram.senderAddress();
            quint16 senderPort = datagram.senderPort();
            qDebug() << "BroadcastDiscoveryService | Received Datagram | " << senderAddress.toString() << ":" << senderPort << " | "<< message;
            if (message == discoveryMessage && senderPort == discoveryPort) {
                emit instanceDiscovered(senderAddress, senderPort);
            }
        }
    }

private:
    const int discoveryPort;
    QByteArray discoveryMessage { "Instance Discovery" };
    QSharedPointer<QTimer> broadcastTimer;
    QSharedPointer<QUdpSocket> udpSocket;
};
*/
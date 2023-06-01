// NetworkManager.cpp
#include "NetworkManager.h"
#include <QDebug>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
    , interfaceSocketFactory(QNetworkInterface::interfaceFromName("eth0"))
{
    // Connect the discovery timer timeout signal to handle the peer discovery process
    connect(&discoveryTimer, &QTimer::timeout, [&]() {
        QString interfaceName = interfaceSocketFactory.getInterfaceName();
        QSharedPointer<InterfaceSocket> interfaceSocket = interfaceSocketFactory.createInterfaceSocket(true);

        // Handle peer discovery for each received data
        connect(interfaceSocket.data(), &InterfaceSocket::dataReceived, this, &NetworkManager::handlePeerDiscovery);

        // Start listening on the interface socket for peer discovery
        interfaceSocket->startListening(QHostAddress::AnyIPv4, 3103);

        // Broadcast discovery message
        interfaceSocket->getUdpSocket()->writeDatagram("Instance Discovery", QHostAddress::Broadcast, 3103);

        // Stop listening and cleanup after the discovery timeout
        QTimer::singleShot(2000, [&]() {
            interfaceSocket->getUdpSocket()->disconnect();
            interfaceSocket->getUdpSocket()->close();
            interfaceSocket->disconnect();
            interfaceSocket.clear();
        });
    });
}

NetworkManager::~NetworkManager()
{
    stopDiscovery();
    stopAudioStreaming();
}

void NetworkManager::startDiscovery()
{
    if (!discoveryTimer->isActive()) {
        discoveryTimer.start(5000); // Start discovery every 5 seconds
    }
}

void NetworkManager::stopDiscovery()
{
    if (discoveryTimer.isActive()) {
        discoveryTimer.stop();
    }
}

void NetworkManager::connectToPeer(int index)
{
    if (index >= 0 && index < peers.size()) {
        QSharedPointer<NetworkPeer> peer = peers.at(index);
        if (!peer->isConnected()) {
            peer->connectToPeer();
        }
    }
}

void NetworkManager::disconnectFromPeer(int index)
{
    if (index >= 0 && index < peers.size()) {
        QSharedPointer<NetworkPeer> peer = peers.at(index);
        if (peer->isConnected()) {
            peer->disconnectFromPeer();
        }
    }
}

void NetworkManager::startAudioStreaming()
{
    // Start audio streaming for all connected peers
    for (const auto& peer : peers) {
        if (peer->isConnected()) {
            peer->startAudioStreaming();
        }
    }
}

void NetworkManager::stopAudioStreaming()
{
    // Stop audio streaming for all connected peers
    for (const auto& peer : peers) {
        if (peer->isConnected()) {
            peer->stopAudioStreaming();
        }
    }
}

void NetworkManager::handlePeerDiscovery(QString name, QString address)
{
    // Check if the peer is already in the list
    for (int i = 0; i < peers.size(); i++) {
        QSharedPointer<NetworkPeer> peer = peers.at(i);
        if (peer->getPeerAddress() == address) {
            // The peer is already in the list, update its connection status
            emit connectionStatusUpdated(i, peer->isConnected());
            return;
        }
    }

    // Create a new NetworkPeer for the discovered peer
    QSharedPointer<InterfaceSocket> interfaceSocket = interfaceSocketFactory.createInterfaceSocket(false);
    QSharedPointer<NetworkPeer> newPeer(new NetworkPeer(interfaceSocket, address));
    peers.append(newPeer);

    // Connect the NetworkPeer's connection status update signal to emit the corresponding signal
    connect(newPeer.data(), &NetworkPeer::connectionStatusUpdated, [&](bool connected) {
        int index = peers.indexOf(newPeer);
        emit connectionStatusUpdated(index, connected);
    });

    // Emit the peer discovered signal
    emit peerDiscovered(name, address);
}

void NetworkManager::handleDataReceived(QByteArray data, QHostAddress sender, quint16 senderPort)
{
    // Forward the received data to the appropriate NetworkPeer
    for (const auto& peer : peers) {
        if (peer->getPeerAddress() == sender.toString()) {
            peer->handleDataReceived(data);
            return;
        }
    }
}

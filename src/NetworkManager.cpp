// NetworkManager.cpp
#include <QDebug>
#include <QNetworkInterface>
#include <QSharedPointer>
#include <QRandomGenerator>
#include "NetworkManager.h"

QSharedPointer<Message> MsgQueue::get()
{
    if (msgs.empty())
    {
        return nullptr;
    }
    QSharedPointer<Message> msg = serial.read(msgs.front());
    msgs.pop_front();
    return msg;
}

void MsgQueueProcessor::processQueue()
{
    while (!queue.empty())
    {
        if (const auto& receivedMessage = queue.get(); receivedMessage)
        {
            if (const auto& outgoingMessage = processMsg(receivedMessage); outgoingMessage)
            {
                QSharedPointer<QByteArray> outgoingMessageBytes = serial.write(outgoingMessage);
                emit outgoingMessageReady(outgoingMessageBytes);
            }
        }
    }
}

QSharedPointer<Message> MsgQueueProcessor::processMsg(QSharedPointer<Message> message)
{
    switch (message->getType())
    {
    case MessageType::PeerDiscoveryRequest:
    {
        const auto& messageData = message->getData().dynamicCast<PeerDiscoveryRequest>();
        emit peerDiscovered(messageData->ssrcId, messageData->svcAnnounces);
        const auto& responseMsg = QSharedPointer<PeerDiscoveryResponse>::create(localSsrcId);
        return responseMsg.staticCast<Message>();
    }
    case MessageType::PeerDiscoveryResponse:
    {
        const auto& messageData = message->getData().dynamicCast<PeerDiscoveryResponse>();
        remoteSsrcIds.push_back(messageData->ssrcId); 
        return nullptr;
    }
    case MessageType::DeviceInfoRequest:
    {
        const auto& messageData = message->getData().dynamicCast<DeviceInfoRequest>();
        // TODO: Replace with actual device type and hardware
        const auto& responseMsg = QSharedPointer<DeviceInfoResponse>::create(DeviceType::Unknown, {});
        return responseMsg.staticCast<Message>();
    }
    case MessageType::DeviceInfoResponse:
    {
        const auto& messageData = message->getData().dynamicCast<DeviceInfoResponse>();
        // TODO: Store the received device info
        return nullptr;
    }
    case MessageType::StartAudioStreamRequest:
    case MessageType::StartAudioStreamResponse:
    {
        auto audioMessage = message->getData().dynamicCast<AudioMessage>();
        if (audioMessage) {
            emit processAudioMessage(audioMessage);
        }
        break;
    }
    default:
        return nullptr;
    }
}

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
{
    ssrcId = QRandomGenerator::global()->generate();
    connect(&msgQueueProcessor, &MsgQueueProcessor::processAudioMessage, this, &NetworkManager::handleAudioMessage);
    connect(&discoveryTimer, &QTimer::timeout, [&]() 
    {
        // Start listening on the interface socket for peer discovery
        interfaceSocket->startListening(QHostAddress::AnyIPv4, 3103);
        // Broadcast discovery message
        interfaceSocket->getUdpSocket()->writeDatagram("Instance Discovery", QHostAddress::Broadcast, 3103);
        // Stop listening and cleanup after the discovery timeout
        QTimer::singleShot(2000, [&]() 
        {
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
        discoveryTimer.start(5000);
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
    for (const auto& peer : peers) {
        if (peer->isConnected()) {
            peer->startAudioStreaming();
        }
    }
}

void NetworkManager::stopAudioStreaming()
{
    for (const auto& peer : peers) {
        if (peer->isConnected()) {
            peer->stopAudioStreaming();
        }
    }
}

void NetworkManager::handlePeerDiscovery(QString name, QString address)
{
    for (int i = 0; i < peers.size(); i++) {
        QSharedPointer<NetworkPeer> peer = peers.at(i);
        if (peer->getPeerAddress() == address) {
            emit connectionStatusUpdated(i, peer->isConnected());
            return;
        }
    }

    QSharedPointer<InterfaceSocket> interfaceSocket = audioServiceFactory.createInterfaceSocket(false);
    QSharedPointer<NetworkPeer> newPeer(new NetworkPeer(interfaceSocket, address));
    peers.append(newPeer);

    connect(newPeer.data(), &NetworkPeer::connectionStatusUpdated, [&](bool connected) {
        int index = peers.indexOf(newPeer);
        emit connectionStatusUpdated(index, connected);
    });

    emit peerDiscovered(name, address);
}

void NetworkManager::handleDataReceived(QByteArray data, QHostAddress sender, quint16 senderPort)
{
    for (const auto& peer : peers) {
        if (peer->getPeerAddress() == sender.toString()) {
            peer->handleDataReceived(data);
            return;
        }
    }
}

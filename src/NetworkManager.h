// NetworkManager.h
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QByteArray>
#include <QIODevice>
#include <QMap>
#include "Message.h"
#include "AudioServiceFactory.h"
#include "AudioService.h"

class MsgQueue : public QObject
{
    Q_OBJECT

public:
    MsgQueue(MessageSerial& serial) : serial(serial) {}
    virtual ~MsgQueue() = default;
    void push(QSharedPointer<QByteArray> bytes) { 
        msgs.push_back(bytes); 
    }
    bool empty() { 
        return msgs.empty();
    }
    QSharedPointer<Message> get();
private:
    std::list<QSharedPointer<QByteArray>> msgs;
    MessageSerial& serial;
};

class MsgQueueProcessor : public QObject
{
    Q_OBJECT
    
public:
    MsgQueueProcessor(MsgQueue& queue, MessageSerial& serial, SsrcId localSsrcId): queue(queue), serial(serial), localSsrcId(localSsrcId)
    {
        connect(&audioService, &AudioService::messageReady, this, &MsgQueueProcessor::audioMessageReady);
        connect(this, &MsgQueueProcessor::processAudioMessage, &audioService, &NetworkManager::handleAudioMessage);
    }
    void processQueue();
    QSharedPointer<Message> processMsg(QSharedPointer<Message> message);
signals:
    void peerDiscovered(SsrcId ssrcId, std::vector<ServiceType> svcAnnounces);
    void audioMessageReady(QSharedPointer<AudioMessage> outgoingMessageBytes);
    void processAudioMessage(QSharedPointer<AudioMessage> audioMessage);
private:
    MsgQueue& queue;
    MessageSerial& serial;
    AudioService& audioService;
};

class NetworkReaderWriter : public QObject
{
    Q_OBJECT

public:
    explicit NetworkReaderWriter(const QHostAddress& address, quint16 port, QObject* parent = nullptr)
        : QObject(parent)
        , address(address)
        , port(port)
    {
        socket.bind(address, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        connect(&socket, &QUdpSocket::readyRead, this, &NetworkReaderWriter::onReadyRead);
    }
    virtual ~NetworkReaderWriter() {}
private slots:
    void NetworkReaderWriter::onReadyRead()
    {
        while (socket.hasPendingDatagrams()) 
        {
            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            
            data.resize(int(socket.pendingDatagramSize()));
            socket.readDatagram(data.data(), data.size(), &sender, &senderPort);

            qDebug("NetworkReaderWriter::onReadyRead data:|%s| size:|%lli|", data.data(), data.size());
            msgQueue.push(QSharedPointer<QByteArray>(new QByteArray(data)));
        }
    }
protected:
    QUdpSocket socket;
    MsgQueue& msgQueue;
};

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager::NetworkManager(QObject* parent = nullptr)
        : QObject(parent)
    {
        connect(&msgQueueProcessor, &MsgQueueProcessor::processAudioMessage, this, &NetworkManager::handleAudioMessage);
    }
    virtual ~NetworkManager();
    void startDiscovery();
    void stopDiscovery();
    void connectToPeer(int index);
    void disconnectFromPeer(int index);
    void startAudioStreaming();
    void stopAudioStreaming();
    QSharedPointer<AudioService> getAudioService(quint16 port) { return audioServices[port]; }
public slots:
    void sendData(QByteArray data, QHostAddress receiver, quint16 receiverPort);
    void handleAudioMessage(QSharedPointer<AudioMessage> audioMessage)
    {
        if (audioServices.find(audioMessage->port) == audioServices.end()) {
            audioServices[audioMessage->port] = audioServiceFactory.createAudioService(audioMessage->port);
        }
        else {
            audioServices[audioMessage->port]->handleAudioMessage(audioMessage);
        }
    }
signals:
    void peerDiscovered(SsrcId ssrcId, ServiceType peerServices);
    void connectionStatusUpdated(int index, bool connected);
    void audioMessageReceived(QSharedPointer<AudioMessage> audioMessage);
private slots:
    void handlePeerDiscovery(QString name, QString address);
    void handleDataReceived(QByteArray data, QHostAddress sender, quint16 senderPort);

private:
    QMap<quint16, QSharedPointer<AudioService> > audioServices;
    AudioServiceFactory audioServiceFactory;
    MsgQueueProcessor& msgQueueProcessor;
    SsrcId ssrcId;
    QTimer* discoveryTimer;
};

#endif // NETWORKMANAGER_H

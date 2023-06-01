// NetworkManager.h
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QByteArray>
#include <QIODevice>
#include <QMap>
#include "NetworkPeer.h"
#include "Message.h"
#include "Job.h"

class MsgQueue
{
    Q_OBJECT
public:
    virtual ~MsgQueue() = default;
    void push(std::shared_ptr<Message> msg) { messages.push_back(msg); }
    void push(QSharedPointer<QByteArray> bytes) { messages.push_back(read(bytes)); }
    std::shared_ptr<Message> get()
    {
        if (messages.empty())
        {
            return nullptr;
        }
        std::shared_ptr<Message> msg = messages.front();
        messages.pop_front();
        return msg;
    }
    bool empty() { return messages.empty(); }

    std::shared_ptr<Message> read(QSharedPointer<QByteArray> bytes)
    {
        if (bytes->size() <= 0) {
            qDebug("RequestReader::read invalid msg size");
            return nullptr;
        }
        QDataStream stream(bytes.data(), QIODevice::ReadOnly);
        if (stream.atEnd())
            return nullptr;
        MessageType type;
        stream >> type;
        if (stream.status() != QDataStream::Ok)
            return nullptr;

        switch (type)
        {
        case MessageType::PeerDiscoveryRequest:
        {
            SsrcId ssrcId;
            int32_t svcAnnouncesSize;
            stream >> svcAnnouncesSize;
            std::vector<ServiceType> svcAnnounces{0, svcAnnouncesSize};
            stream >> ssrcId;
            for (auto& item : svcAnnounces) {
                stream >> item;
            }
            const auto& messageData = std::make_shared<PeerDiscoveryRequest>(ssrcId, svcAnnounces);
            return std::make_shared<Message>(type, messageData);
        }
        case MessageType::PeerDiscoveryResponse:
        {
            SsrcId ssrcId;
            stream >> ssrcId;
            const auto& messageData = std::make_shared<PeerDiscoveryResponse>(ssrcId);
            return std::make_shared<Message>(type, messageData);
        }
        case MessageType::DeviceInfoRequest:
        {
            SsrcId ssrcId;
            stream >> ssrcId;
            const auto& messageData = std::make_shared<DeviceInfoRequest>(ssrcId);
            return std::make_shared<Message>(type, messageData);
        }
        case MessageType::DeviceInfoResponse:
        {
            DeviceType deviceType;
            stream >> deviceType;
            int32_t deviceHardwareSize;
            stream >> deviceHardwareSize;
            std::vector<int32_t> deviceHardware{0, deviceHardwareSize};
            for (auto& item : deviceHardware) {
                stream >> item;
            }
            const auto& messageData = std::make_shared<DeviceInfoResponse>(deviceType, deviceHardware);
            return std::make_shared<Message>(type, messageData);
        }
        default:
            return nullptr;
        }
    }
private:
    std::list<std::shared_ptr<Message>> messages;
};

class MsgQueueProcessor
{
    Q_OBJECT
public:
    MsgQueueProcessor(MsgQueue& queue): queue(queue) {}
    void processQueue()
    {
        while (!queue.empty())
        {
            if (const auto& receivedMessage = queue.get(); receivedMessage)
            {
                if (const auto& outgoingMessage = processMsg(receivedMessage); outgoingMessage)
                {
                    QSharedPointer<QByteArray> outgoingMessageBytes = write(outgoingMessage);
                    //TODO: emit signal outgoingMessageReady(outgoingMessageBytes)
                }
            }
        }
    }

    QSharedPointer<QByteArray> write(std::shared_ptr<Message> message)
    {
        QByteArray bytes;
        QDataStream stream(&bytes, QIODevice::WriteOnly);
        stream << message->getType();
        switch (message->getType())
        {
        case MessageType::PeerDiscoveryRequest:
        {
            const auto& data = std::static_pointer_cast<PeerDiscoveryRequest>(message->getData());
            stream << data->ssrcId;
            for (const auto& item : data->svcAnnounces) {
                stream << item;
            }
            return QSharedPointer<QByteArray>(&bytes);
        }
        case MessageType::PeerDiscoveryResponse:
        {
            const auto& data = std::static_pointer_cast<PeerDiscoveryResponse>(message->getData());
            stream << data->ssrcId;
            return QSharedPointer<QByteArray>(&bytes);
        }
        case MessageType::DeviceInfoRequest:
        {
            const auto& data = std::static_pointer_cast<DeviceInfoRequest>(message->getData());
            stream << data->ssrcId;
            return QSharedPointer<QByteArray>(&bytes);
        }
        case MessageType::DeviceInfoResponse:
        {
            const auto& data = std::static_pointer_cast<DeviceInfoResponse>(message->getData());
            stream << data->deviceType;
            for (const auto& item : data->deviceHardware) {
                stream << item;
            }
            return QSharedPointer<QByteArray>(&bytes);
        }
        default:
            return QSharedPointer<QByteArray>(new QByteArray());
        }
    }
    std::shared_ptr<Message> processMsg(std::shared_ptr<Message> message)
    {
        switch (message->getType())
            {
            case MessageType::PeerDiscoveryRequest:
            {
                const auto& messageData = std::reinterpret_pointer_cast<PeerDiscoveryRequest>(message->getData());
                int32_t startId = messageData->ssrcId + 1;
                bool uniqueIdFound = true;
                int32_t thisSsrcId = 0;
                while (thisSsrcId == 0)
                {
                    for (const auto& id : messageData->svcAnnounces) {
                        if (startId == id) {
                            uniqueIdFound = false;
                            break;
                        }
                    }
                    if (uniqueIdFound) {
                        thisSsrcId = startId;
                        break;
                    }
                    startId++;
                    uniqueIdFound = true;
                }
                emit thisSsrcIdSet(thisSsrcId, messageData->ssrcId);
                const auto& responseMsg = std::make_shared<PeerDiscoveryResponse>(thisSsrcId);
                return std::dynamic_pointer_cast<Message>(responseMsg);
            }
            case MessageType::PeerDiscoveryResponse:
            {
                const auto& messageData = std::reinterpret_pointer_cast<PeerDiscoveryResponse>(message->getData());

                if (messageData->ssrcId == 0)
                {
                    return nullptr;
                }
                messageData->ssrcId
                const auto& responseMsg = std::make_shared<PeerDiscoveryResponse>();
                return std::dynamic_pointer_cast<Message>(responseMsg);
            }
            case MessageType::DeviceInfoRequest:
            {
                const auto& messageData = std::reinterpret_pointer_cast<DeviceInfoRequest>(message->getData());
                const auto& responseMsg = std::make_shared<DeviceInfoRequest>(message->getData());
                return std::dynamic_pointer_cast<Message>(responseMsg);
            }
            case MessageType::DeviceInfoResponse:
            {
                const auto& messageData = std::reinterpret_pointer_cast<DeviceInfoResponse>(message->getData());
                stream << data->deviceType;
                for (const auto& item : data->deviceHardware) {
                        stream << item;
                }

                const auto& responseMsg = std::make_shared<DeviceInfoResponse>(message->getData());
                return std::dynamic_pointer_cast<Message>(responseMsg);
            }
            default:
                return nullptr;
        }
    }
private slots:
    void addRemoteSsrcId(int thisSsrcId, int remoteSsrcId)
    {
        if (localSsrcId == 0)
        {
            localSsrcId = thisSsrcId
            connectedDevices[thisSsrcId] = QList<int32_t>
        }
        if (const auto& found = connectedDevices.find(remoteSsrcId); found == connectedDevices.end())
        {
            connectedDevices[thisSsrcId].append(remoteSsrcId);
        }
        ssrcIds[thisSsrcId] = remoteSsrcId;
    }
signals:
    void thisSsrcIdSet(int thisSsrcId, int remoteSsrcId);

private:
    MsgQueue& queue;
    QMap<int32_t, QList<int32_t> > connectedDevices{};
    int32_t localSsrcId = 0;
    int32_t jobId = 0;
};
/*
Connects the interface socket's data received signal to the corresponding network manager slots.
Connects the audio service's data received signal to the corresponding network manager slots.
*/
class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject* parent = nullptr);
    virtual ~NetworkManager();

    void startDiscovery();
    void stopDiscovery();
    void connectToPeer(int index);
    void disconnectFromPeer(int index);
    void startAudioStreaming();
    void stopAudioStreaming();

signals:
    void peerDiscovered(QString name, QString address);
    void connectionStatusUpdated(int index, bool connected);

private slots:
    void handlePeerDiscovery(QString name, QString address);
    void handleDataReceived(QByteArray data, QHostAddress sender, quint16 senderPort);

private:
    QMap<QSharedPointer<NetworkPeer>, QSharedPointer<NetworkPeer>> peers;
    InterfaceSocketFactory interfaceSocketFactory;
    QTimer* discoveryTimer;

    quint32 ssrc = QRandomGenerator::global()->generate();

};

#endif // NETWORKMANAGER_H

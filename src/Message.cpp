#include <QDataStream>
#include <QDebug>
#include "Message.h"

std::shared_ptr<Message> MessageSerial::read(QSharedPointer<QByteArray> bytes)
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
    case MessageType::StartAudioStreamRequest:
    case MessageType::StartAudioStreamResponse:
    case MessageType::StopAudioStreamRequest:
    case MessageType::StopAudioStreamResponse:
    {
        quint16 port;
        stream >> port;
        auto audioMsg = std::make_shared<AudioMessage>(port);
        msg = std::make_shared<Message>(type, audioMsg);
        break;
    }
    default:
        return nullptr;
    }
}

QSharedPointer<QByteArray> MessageSerial::write(QSharedPointer<Message> message)
{
    auto bytes = QSharedPointer<QByteArray>(new QByteArray());
    QDataStream stream(bytes.get(), QIODevice::WriteOnly);
    stream << message->getType();
    switch (message->getType())
    {
    case MessageType::PeerDiscoveryRequest:
    {
        const auto& data = message->getData().dynamicCast<PeerDiscoveryRequest>();
        stream << data->ssrcId;
        stream << static_cast<int32_t>(data->svcAnnounces.size());
        for (const auto& item : data->svcAnnounces) {
            stream << item;
        }
        break;
    }
    case MessageType::PeerDiscoveryResponse:
    {
        const auto& data = message->getData().dynamicCast<PeerDiscoveryResponse>();
        stream << data->ssrcId;
        break;
    }
    case MessageType::DeviceInfoRequest:
    {
        const auto& data = message->getData().dynamicCast<DeviceInfoRequest>();
        stream << data->ssrcId;
        break;
    }
    case MessageType::DeviceInfoResponse:
    {
        const auto& data = message->getData().dynamicCast<DeviceInfoResponse>();
        stream << data->deviceType;
        stream << static_cast<int32_t>(data->deviceHardware.size());
        for (const auto& item : data->deviceHardware) {
            stream << item;
        }
        break;
    }
    case MessageType::StartAudioStreamRequest:
    case MessageType::StartAudioStreamResponse:
    case MessageType::StopAudioStreamRequest:
    case MessageType::StopAudioStreamResponse:
    {
        auto audioMsg = std::dynamic_pointer_cast<AudioMessage>(message->getData());
        if (audioMsg) {
            stream << audioMsg->port;
        }
        break;
    }
    default:
        break;
    }
    return bytes;
}

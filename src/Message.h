// Message.h
#ifndef MESSAGE_H
#define MESSAGE_H
#include <memory>
#include <vector>
#include <QSharedPointer>
#include <QByteArray>

using SsrcId = int32_t;

enum class MessageType {
    Unknown = 0,
    PeerDiscoveryRequest,
    PeerDiscoveryResponse,
    DeviceInfoRequest,
    DeviceInfoResponse,
    StartAudioStreamRequest,
    StartAudioStreamResponse,
    StopAudioStreamRequest,
    StopAudioStreamResponse,
};

enum class DeviceType {
    Unknown = 0,
    Linux = 1,
    Windows = 2,
    Android = 3,
};

enum class HardwareType {
    Unknown = 0,
    Wifi_24 = 1,
    Wifi_5 = 2,
    Bluetooth_4 = 3,
    Bluetooth_5 = 4,
    AudioOutput = 5,
};

enum class ServiceType {
    Unknown = 0,
    TimeServer = 1,
    MediaServer = 2,
    WebRTC = 3,
    Repeater = 4,
};

class IMessageData
{
public:
    virtual ~IMessageData() = default;
};

class PeerDiscoveryRequest: public IMessageData
{
public:
    explicit PeerDiscoveryRequest(SsrcId ssrcId, std::vector<ServiceType> svcAnnounces): ssrcId(ssrcId), svcAnnounces(svcAnnounces)
    {}
    virtual ~PeerDiscoveryRequest() override = default;
    SsrcId ssrcId;
    size_t svcAnnouncesSize() { return svcAnnounces.size(); }
    std::vector<ServiceType> svcAnnounces;
};

class PeerDiscoveryResponse : public IMessageData
{
public:
    explicit PeerDiscoveryResponse(SsrcId ssrcId): ssrcId(ssrcId)
    {}
    virtual ~PeerDiscoveryResponse() override = default;
    SsrcId ssrcId;
};

class DeviceInfoRequest: public IMessageData
{
public:
    explicit DeviceInfoRequest(SsrcId ssrcId): ssrcId(ssrcId)
    {}
    virtual ~DeviceInfoRequest() override = default;
    SsrcId ssrcId; // ID of device the info is requested about.
};

class DeviceInfoResponse : public IMessageData
{
public:
    explicit DeviceInfoResponse(DeviceType deviceType, std::vector<HardwareType> deviceHardware = {}): deviceType(deviceType), deviceHardware(deviceHardware)
    {}
    virtual ~DeviceInfoResponse() override = default;
    DeviceType deviceType;
    size_t deviceHardwareSize() { return deviceHardware.size(); }
    std::vector<HardwareType> deviceHardware;
};

class AudioMessage : public IMessageData {
public:
    explicit AudioMessage(quint16 port = 3101 /* TODO: temporary static value for port */): port(port) {}
    virtual ~AudioMessage() = default;
    quint16 port;
};

class Message
{
public:
    Message(MessageType type, QSharedPointer<IMessageData> data): type(type), data(data)
    {}
    Message(MessageType type, std::shared_ptr<IMessageData> data): type(type), data(&*data)
    {}
    Message(MessageType type, const IMessageData& data): type(type), data(QSharedPointer<IMessageData>::create(data))
    {}
    virtual ~Message() = default;
    MessageType getType() const { return type; }
    QSharedPointer<IMessageData> getData() const { return data; }
protected:
    MessageType type;
    QSharedPointer<IMessageData> data;
};
// MessageSerial is providing serialization of outgoing messages and deserialization of incoming messages.
class MessageSerial
{
public:
    MessageSerial() = default;
    virtual ~MessageSerial() = default;
    std::shared_ptr<Message> read(QSharedPointer<QByteArray> bytes);
    QSharedPointer<QByteArray> write(QSharedPointer<Message> message);
private:
    std::shared_ptr<Message> message;
};

/*
class StartAudioStreamRequest : public AudioMessage {
    Q_OBJECT
public:
    explicit StartAudioStreamRequest(QSharedPointer<QIODevice> targetDevice, QObject* parent = nullptr)
        : AudioMessage(parent), targetDevice(targetDevice) {}

    QSharedPointer<QIODevice> getTargetDevice() const { return targetDevice; }

private:
    QSharedPointer<QIODevice> targetDevice;
};
// Define a StartAudioStreamer message
class StartAudioStreamResponse : public AudioMessage {
    Q_OBJECT
public:
    explicit StartAudioStreamResponse(QSharedPointer<QIODevice> targetDevice, QObject* parent = nullptr)
        : AudioMessage(parent), targetDevice(targetDevice) {}

    QSharedPointer<QIODevice> getTargetDevice() const { return targetDevice; }

private:
    QSharedPointer<QIODevice> targetDevice;
};

// Define a StopAudioStreamer message
class StopAudioStreamMessage : public AudioMessage {
    Q_OBJECT
public:
    explicit StopAudioStreamerMessage(QObject* parent = nullptr) : AudioMessage(parent) {}
};
*/
#endif // MESSAGE_H





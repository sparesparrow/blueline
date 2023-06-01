// Message.h
#ifndef MESSAGE_H
#define MESSAGE_H
#include <memory>
#include <vector>

using SsrcId = int32_t;

enum class MessageType {
    Unknown = 0,
    PeerDiscoveryRequest,
    PeerDiscoveryResponse,
    DeviceInfoRequest,
    DeviceInfoResponse,
    // TODO More request types...
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

class Message
{
public:
    Message(MessageType type, std::shared_ptr<IMessageData> data): type(type), data(data)
    {}
    virtual ~Message() = default;
    MessageType getType() const { return type; }
    std::shared_ptr<IMessageData> getData() const { return data; }
    int32_t getSize() const { return sizeof(data); }
protected:
    MessageType type;
    std::shared_ptr<IMessageData> data;
};

#endif // MESSAGE_H

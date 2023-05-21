#ifndef DEVICE_H
#define DEVICE_H

#include <string>

enum class DeviceType { AUDIO_INPUT, AUDIO_OUTPUT, UNKNOWN };
enum class ConnectionStatus { DISCONNECTED, CONNECTED, IN_USE };

class Device {
public:
    Device(std::string id, std::string name, DeviceType type);

    std::string getId() const;
    std::string getName() const;
    DeviceType getType() const;
    ConnectionStatus getStatus() const;

    void setStatus(ConnectionStatus status);

private:
    std::string id;
    std::string name;
    DeviceType type;
    ConnectionStatus status;
};

#endif // DEVICE_H

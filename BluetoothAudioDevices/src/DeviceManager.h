// DeviceManager.h
#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <vector>
#include <unordered_map>
#include <map>
#include <optional>
#include "Device.h"

class DeviceManager {
public:
    void addDevice(const Device& device);
    bool removeDevice(const std::string& id);
    std::optional<Device> getDevice(const std::string& id) const;
    std::vector<Device> getAllDevices() const;

private:
    std::vector<Device> devices;
};

#endif // DEVICEMANAGER_H

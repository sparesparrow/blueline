// DeviceManager.cpp
#include "DeviceManager.h"
#include <algorithm>
#include <stdexcept>

void DeviceManager::addDevice(const Device& device) {
    // Check if device already exists
    auto it = std::find_if(devices.begin(), devices.end(), [&device](const Device& d) {
        return d.getId() == device.getId();
    });

    if (it != devices.end()) {
        throw std::runtime_error("Device already exists");
    }

    devices.push_back(device);
}

bool DeviceManager::removeDevice(const std::string& id) {
    auto it = std::find_if(devices.begin(), devices.end(), [&id](const Device& d) {
        return d.getId() == id;
    });

    if (it != devices.end()) {
        devices.erase(it);
        return true;
    }

    return false;
}

std::optional<Device> DeviceManager::getDevice(const std::string& id) const {
    auto it = std::find_if(devices.begin(), devices.end(), [&id](const Device& d) {
        return d.getId() == id;
    });

    if (it != devices.end()) {
        return *it;
    }

    return std::nullopt;
}

std::vector<Device> DeviceManager::getAllDevices() const {
    return devices;
}

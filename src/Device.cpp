#include "Device.h"

Device::Device(std::string id, std::string name, DeviceType type) 
    : id(std::move(id)), name(std::move(name)), type(type), status(ConnectionStatus::DISCONNECTED) {}

std::string Device::getId() const {
    return this->id;
}

std::string Device::getName() const {
    return this->name;
}

DeviceType Device::getType() const {
    return this->type;
}

ConnectionStatus Device::getStatus() const {
    return this->status;
}

void Device::setStatus(ConnectionStatus status) {
    this->status = status;
}

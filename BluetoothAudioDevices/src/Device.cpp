#include "Device.h"

Device::Device(QString id, QString name, QAudioDevice::Mode mode, QObject *parent)
    : QObject(parent), id(std::move(id)), name(std::move(name)), mode(mode), status(ConnectionStatus::DISCONNECTED) {}

QString Device::getId() const {
    return this->id;
}

QString Device::getName() const {
    return this->name;
}

DeviceMode Device::getMode() const {
    return this->mode;
}

ConnectionStatus Device::getStatus() const {
    return this->status;
}

void Device::setStatus(ConnectionStatus status) {
    if (this->status != status) {
        this->status = status;
        emit statusChanged();
    }
}

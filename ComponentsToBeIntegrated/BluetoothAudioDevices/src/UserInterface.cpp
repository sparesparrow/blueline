// UserInterface.cpp
#include "UserInterface.h"
#include <QVariant>

void UserInterface::pairDevice(const QString& deviceId) {
    std::optional<Device> device = m_deviceManager->getDevice(deviceId.toStdString());
    if (device.has_value())
        m_bluetoothManager->pairDevice(device.value());
}

void UserInterface::exchangeData(const QString& deviceId, const QString& data) {
    std::optional<Device> device = m_deviceManager->getDevice(deviceId.toStdString());
    if (device.has_value())
        m_bluetoothManager->exchangeData(device.value(), data.toStdString());
}

void UserInterface::addDevice(const QString& deviceId, const QString& name, int deviceType) {
    Device device(deviceId.toStdString(), name.toStdString(), static_cast<DeviceType>(deviceType));
    m_deviceManager->addDevice(device);
}

void UserInterface::removeDevice(const QString& deviceId) {
    m_deviceManager->removeDevice(deviceId.toStdString());
}

QVariant UserInterface::getDevice(const QString& deviceId) {
    std::optional<Device> device = m_deviceManager->getDevice(deviceId.toStdString());
    if (device.has_value())
        return QVariant::fromValue(device.value());
    else
        return QVariant();
}

void UserInterface::bindAudio(const QVariant& inputDevice, const QVariant& outputDevice) {
    Device input = qvariant_cast<Device>(inputDevice);
    Device output = qvariant_cast<Device>(outputDevice);
    m_audioIOManager->bindAudio(input, output);
}

void UserInterface::updateDeviceModel() {
    std::vector<Device> devices = m_deviceManager->getDevices();
    m_deviceModel->setDeviceList(devices);
}

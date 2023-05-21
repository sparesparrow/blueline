
// UserInterface.cpp
#include "UserInterface.h"

// DeviceListModel implementation
DeviceListModel::DeviceListModel(QObject* parent) : QAbstractListModel(parent) {}

int DeviceListModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return m_devices.size();
}

QVariant DeviceListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    const Device& device = m_devices[index.row()];

    switch (role) {
        case IdRole:
            return QVariant(device.getId().c_str());
        case NameRole:
            return QVariant(device.getName().c_str());
        case TypeRole:
            return QVariant(static_cast<int>(device.getType()));
        case StatusRole:
            return QVariant(static_cast<int>(device.getStatus()));
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> DeviceListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "deviceId";
    roles[NameRole] = "deviceName";
    roles[TypeRole] = "deviceType";
    roles[StatusRole] = "deviceStatus";
    return roles;
}

void DeviceListModel::setDeviceList(const std::vector<Device>& devices) {
    beginResetModel();
    m_devices = devices;
    endResetModel();
}


// UserInterface implementation
UserInterface::UserInterface(BluetoothManager* bluetoothManager, DeviceManager* deviceManager, AudioIOManager* audioIOManager, QObject* parent)
    : QObject(parent), m_bluetoothManager(bluetoothManager), m_deviceManager(deviceManager), m_audioIOManager(audioIOManager) {
    m_deviceModel = new DeviceListModel(this);
}

void UserInterface::discoverDevices() {
    m_bluetoothManager->discoverDevices();
    updateDeviceModel();
}

void UserInterface::pairDevice(const QString& deviceId) {
    m_bluetoothManager->pairDevice(m_deviceManager->getDevice(deviceId));
    updateDeviceModel();
}

void UserInterface::exchangeData(const QString& deviceId, const QString& data) {
    m_bluetoothManager->exchangeData(m_deviceManager->getDevice(deviceId), data.toStdString());
}

void UserInterface::addDevice(const QString& deviceId, const QString& name, int type) {
    DeviceType deviceType = static_cast<DeviceType>(type);
    Device device(deviceId, name.toStdString(), deviceType);
    m_deviceManager->addDevice(device);
    updateDeviceModel();
}

void UserInterface::removeDevice(const QString& deviceId) {
    m_deviceManager->removeDevice(m_deviceManager->getDevice(deviceId));
    updateDeviceModel();
}

QVariant UserInterface::getDevice(const QString& deviceId) {
    const Device& device = m_deviceManager->getDevice(deviceId);
    QVariantMap deviceMap;
    deviceMap["deviceId"] = device.getId().c_str();
    deviceMap["deviceName"] = device.getName().c_str();
    deviceMap["deviceType"] = static_cast<int>(device.getType());
    deviceMap["deviceStatus"] = static_cast<int>(device.getStatus());
    return QVariant(deviceMap);
}

void UserInterface::bindAudio(const QVariant& inputDevice, const QVariant& outputDevice) {
    QString inputDeviceId = inputDevice.toMap().value("deviceId").toString();
    QString outputDeviceId = outputDevice.toMap().value("deviceId").toString();

    Device input = m_deviceManager->getDevice(inputDeviceId);
    Device output = m_deviceManager->getDevice(outputDeviceId);

    // Perform the binding operation using appropriate C++ classes
    m_audioIOManager->bindInput(input);
    m_audioIOManager->bindOutput(output);

    // Update the device status in the deviceModel
    for (int i = 0; i < m_deviceModel->rowCount(); i++) {
        QModelIndex index = m_deviceModel->index(i, 0);
        QString deviceId = index.data(DeviceListModel::IdRole).toString();
        if (deviceId == inputDeviceId || deviceId == outputDeviceId) {
            m_deviceModel->setData(index, static_cast<int>(ConnectionStatus::IN_USE), DeviceListModel::StatusRole);
        }
    }

    // Emit the signal to notify the UI about the changes
    emit devicesChanged();
}

QVariant UserInterface::getInputDevices() {
    std::vector<Device> inputDevices = m_audioIOManager->getInputDevices();
    QVariantList devicesList;
    for (const auto& device : inputDevices) {
        QVariantMap deviceMap;
        deviceMap["deviceId"] = QVariant(device.getId().c_str());
        deviceMap["deviceName"] = QVariant(device.getName().c_str());
        deviceMap["deviceType"] = QVariant(static_cast<int>(device.getType()));
        deviceMap["deviceStatus"] = QVariant(static_cast<int>(device.getStatus()));
        devicesList.append(deviceMap);
    }
    return QVariant(devicesList);
}

QVariant UserInterface::getOutputDevices() {
    std::vector<Device> outputDevices = m_audioIOManager->getOutputDevices();
    QVariantList devicesList;
    for (const auto& device : outputDevices) {
        QVariantMap deviceMap;
        deviceMap["deviceId"] = QVariant(device.getId().c_str());
        deviceMap["deviceName"] = QVariant(device.getName().c_str());
        deviceMap["deviceType"] = QVariant(static_cast<int>(device.getType()));
        deviceMap["deviceStatus"] = QVariant(static_cast<int>(device.getStatus()));
        devicesList.append(deviceMap);
    }
    return QVariant(devicesList);
}

QAbstractListModel* UserInterface::deviceModel() const {
    return m_deviceModel;
}

void UserInterface::updateDeviceModel() {
    std::vector<Device> devices = m_deviceManager->getDevices();
    m_deviceModel->setDeviceList(devices);
    emit devicesChanged();
}

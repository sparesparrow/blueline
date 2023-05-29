// BluetoothManager.cpp
#include "BluetoothManager.h"
#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>

BluetoothManager::BluetoothManager(QObject *parent)
    : QObject(parent), m_deviceDiscoveryAgent(new QBluetoothDeviceDiscoveryAgent(this)), m_socket(new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this)) {
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothManager::deviceDiscovered);
    connect(m_socket, &QBluetoothSocket::readyRead, this, &BluetoothManager::readSocket);
}

BluetoothManager::~BluetoothManager() {
    delete m_deviceDiscoveryAgent;
    delete m_socket;
}

void BluetoothManager::startDeviceDiscovery() {
    m_deviceDiscoveryAgent->start();
}

void BluetoothManager::pairDevice(const Device& device) {
    QBluetoothLocalDevice localDevice;
    localDevice.requestPairing(QBluetoothAddress(device.getId()), QBluetoothLocalDevice::Paired);
    connect(&localDevice, &QBluetoothLocalDevice::pairingFinished, this, &BluetoothManager::pairingDone);
}

void BluetoothManager::exchangeData(const Device& device, const std::string& data) {
    if(m_socket->state() == QBluetoothSocket::ConnectedState) {
        m_socket->write(data.c_str());
    }
}

void BluetoothManager::deviceDiscovered(const QBluetoothDeviceInfo &device) {
    if(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        // We are interested only in LE (Bluetooth Smart) devices
        return;
    }
    // Here you can handle the discovered devices
}

void BluetoothManager::pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing) {
    if(pairing == QBluetoothLocalDevice::Paired) {
        // If device is paired, connect to it
        m_socket->connectToService(address, QBluetoothUuid::SerialPort);
    }
}

void BluetoothManager::readSocket() {
    // Handle the received data from the device
}

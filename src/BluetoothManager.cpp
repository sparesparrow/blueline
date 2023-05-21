
// BluetoothManager.cpp
#include "BluetoothManager.h"
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceInfo>

BluetoothManager::BluetoothManager(QObject *parent)
    : QObject(parent),
      discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this)),
      socket(new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this)) {

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothManager::onDeviceDiscovered);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothManager::onFinishedDeviceDiscovery);
    connect(socket, &QBluetoothSocket::connected, this, &BluetoothManager::onSocketConnected);
    connect(socket, &QBluetoothSocket::readyRead, this, &BluetoothManager::onSocketReadyRead);
}

BluetoothManager::~BluetoothManager() {
    delete socket;
    delete discoveryAgent;
}

void BluetoothManager::discoverDevices() {
    QBluetoothLocalDevice localDevice;
    if(localDevice.isValid()) {
        qDebug() << "Local device:" << localDevice.name() << '(' << localDevice.address().toString() << ')';
        discoveryAgent->start();
    }
}

void BluetoothManager::onDeviceDiscovered(const QBluetoothDeviceInfo &info) {
    Device device(info.address().toString(), info.name(), DeviceType::UNKNOWN);
    emit deviceDiscovered(device);
}

void BluetoothManager::onFinishedDeviceDiscovery() {
    emit finishedDeviceDiscovery();
}

void BluetoothManager::pairDevice(const Device& device) {
    // Not yet implemented.
    // Implement pairing logic here.
}

void BluetoothManager::exchangeData(const Device& device, const QString& data) {
    socket->connectToService(QBluetoothAddress(device.getId().c_str()), QBluetoothUuid(QBluetoothUuid::Rfcomm), QIODevice::ReadWrite);
    // save data into a member variable
    this->data = data;
}

void BluetoothManager::onSocketConnected() {
    qDebug() << "Socket connected to" << socket->peerName();
    if (socket->state() == QBluetoothSocket::ConnectedState) {
        socket->write(data.toUtf8());
    } else {
        qDebug() << "Failed to connect to the Bluetooth device";
    }
}

void BluetoothManager::onSocketReadyRead() {
    QString data(socket->readAll());
    qDebug() << "Data received:" << data;
    emit dataReceived(data);
}

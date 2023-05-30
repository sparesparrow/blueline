// BluetoothManager.h
#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include "Device.h"

class BluetoothManager : public QObject {
    Q_OBJECT

public:
    BluetoothManager(QObject *parent = nullptr);
    ~BluetoothManager();
    void startDeviceDiscovery();
    void pairDevice(const Device& device);
    void exchangeData(const Device& device, const std::string& data);

private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    void readSocket();

private:
    QBluetoothDeviceDiscoveryAgent* m_deviceDiscoveryAgent;
    QBluetoothSocket* m_socket;
};

#endif // BLUETOOTHMANAGER_H

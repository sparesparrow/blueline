// BluetoothManager.h
#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include "Device.h"

class BluetoothManager : public QObject {
    Q_OBJECT

public:
    explicit BluetoothManager(QObject *parent = nullptr);
    ~BluetoothManager();

    void discoverDevices();
    void pairDevice(const Device& device);
    void exchangeData(const Device& device, const QString& data);

signals:
    void deviceDiscovered(const Device& device);
    void finishedDeviceDiscovery();
    void dataReceived(const QString& data);

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);
    void onFinishedDeviceDiscovery();
    void onSocketConnected();
    void onSocketReadyRead();

private:
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *socket;
    QString data; // add this line
};

#endif // BLUETOOTHMANAGER_H

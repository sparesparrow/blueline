// UserInterface.h
#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QObject>
#include <QAbstractListModel>
#include "BluetoothManager.h"
#include "DeviceManager.h"
#include "AudioIOManager.h"

class DeviceListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum DeviceRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        TypeRole,
        StatusRole
    };

    DeviceListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setDeviceList(const std::vector<Device>& devices);

private:
    std::vector<Device> m_devices;
};

class UserInterface : public QObject {
    Q_OBJECT

public:
    UserInterface(BluetoothManager* bluetoothManager, DeviceManager* deviceManager, AudioIOManager* audioIOManager, QObject* parent = nullptr);

    Q_INVOKABLE void discoverDevices();
    Q_INVOKABLE void pairDevice(const QString& deviceId);
    Q_INVOKABLE void exchangeData(const QString& deviceId, const QString& data);
    Q_INVOKABLE void addDevice(const QString& deviceId, const QString& name, int type);
    Q_INVOKABLE void removeDevice(const QString& deviceId);
    Q_INVOKABLE QVariant getDevice(const QString& deviceId);
    Q_INVOKABLE void bindAudio(const QVariant& inputDevice, const QVariant& outputDevice);
    Q_INVOKABLE QVariant getInputDevices();
    Q_INVOKABLE QVariant getOutputDevices();

    QAbstractListModel* deviceModel() const;

signals:
    void devicesChanged();

private:
    BluetoothManager* m_bluetoothManager;
    DeviceManager* m_deviceManager;
    AudioIOManager* m_audioIOManager;
    DeviceListModel* m_deviceModel;

    void updateDeviceModel();
};

#endif // USERINTERFACE_H

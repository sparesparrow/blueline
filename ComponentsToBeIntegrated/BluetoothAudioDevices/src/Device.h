#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QMediaFormat>
#include <string>


enum class ConnectionStatus { DISCONNECTED, CONNECTED, IN_USE };
Q_ENUMS(ConnectionStatus)

class Device : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString id READ getId NOTIFY idChanged)
    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    Q_PROPERTY(DeviceType mode READ getMode NOTIFY modeChanged)
    Q_PROPERTY(ConnectionStatus status READ getStatus NOTIFY statusChanged)

public:
    Device(QString id, QString name, QAudioDevice::Mode mode, QObject *parent = nullptr);

    QString getId() const;
    QString getName() const;
    QAudioDevice::Mode getMode() const;
    ConnectionStatus getStatus() const;

    void setStatus(ConnectionStatus status);

signals:
    void idChanged();
    void nameChanged();
    void modeChanged();
    void statusChanged();

private:
    QString id;
    QString name;
    ConnectionStatus status;
    QAudioDevice deviceInfo;
    QAudioFormat settings;
    QAudioDevice::Mode mode;
};

#endif // DEVICE_H

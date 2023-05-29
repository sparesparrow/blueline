// NetworkManager.h
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <QObject>
#include <QSharedPointer>
#include <QList>
#include "NetworkPeer.h"

/* 
Connects the interface socket's data received signal to the corresponding network manager slots.
Connects the audio service's data received signal to the corresponding network manager slots.
*/
class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject* parent = nullptr);
    virtual ~NetworkManager();

    void startDiscovery();
    void stopDiscovery();
    void connectToPeer(int index);
    void disconnectFromPeer(int index);
    void startAudioStreaming();
    void stopAudioStreaming();

signals:
    void peerDiscovered(QString name, QString address);
    void connectionStatusUpdated(int index, bool connected);

private slots:
    void handlePeerDiscovery(QString name, QString address);
    void handleDataReceived(QByteArray data, QHostAddress sender, quint16 senderPort);

private:
    QList<QSharedPointer<NetworkPeer>> peers;
    InterfaceSocketFactory interfaceSocketFactory;
    QTimer discoveryTimer;
};

#endif // NETWORKMANAGER_H

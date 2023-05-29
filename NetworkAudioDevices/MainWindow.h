// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "NetworkManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(NetworkManager* networkManager, QWidget* parent = nullptr);
    virtual ~MainWindow();

private slots:
    void startDiscovery();
    void stopDiscovery();
    void connectToPeer();
    void disconnectFromPeer();
    void startAudioStreaming();
    void stopAudioStreaming();
    void handlePeerDiscovered(QString name, QString address);
    void handleConnectionStatusUpdated(int index, bool connected);

signals:
    void startDiscoveryRequested();
    void stopDiscoveryRequested();
    void connectToPeerRequested(QString address);
    void disconnectFromPeerRequested(QString address);
    void startAudioStreamingRequested(QString address);
    void stopAudioStreamingRequested(QString address);

private:
    NetworkManager* networkManager;
    QListWidget* peerListWidget;
    QPushButton* connectButton;
    QPushButton* disconnectButton;
    QPushButton* startStreamingButton;
    QPushButton* stopStreamingButton;
};

#endif // MAINWINDOW_H

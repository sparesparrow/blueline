// MainWindow.cpp
#include "MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(NetworkManager* networkManager, QWidget* parent)
    : QMainWindow(parent)
    , networkManager(networkManager)
{
    // Set up the main window
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    // Create the peer list widget
    peerListWidget = new QListWidget(this);
    layout->addWidget(peerListWidget);

    // Create the connect and disconnect buttons
    connectButton = new QPushButton("Connect", this);
    disconnectButton = new QPushButton("Disconnect", this);
    disconnectButton->setEnabled(false);

    // Create the start and stop streaming buttons
    startStreamingButton = new QPushButton("Start Streaming", this);
    stopStreamingButton = new QPushButton("Stop Streaming", this);
    stopStreamingButton->setEnabled(false);

    // Connect the UI actions to the corresponding slots
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::connectToPeer);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::disconnectFromPeer);
    connect(startStreamingButton, &QPushButton::clicked, this, &MainWindow::startAudioStreaming);
    connect(stopStreamingButton, &QPushButton::clicked, this, &MainWindow::stopAudioStreaming);

    // Add the buttons to the layout
    layout->addWidget(connectButton);
    layout->addWidget(disconnectButton);
    layout->addWidget(startStreamingButton);
    layout->addWidget(stopStreamingButton);

    // Connect the network manager signals to the UI slots
    connect(this, &MainWindow::startDiscoveryRequested, networkManager, &NetworkManager::startDiscovery);
    connect(this, &MainWindow::stopDiscoveryRequested, networkManager, &NetworkManager::stopDiscovery);
    connect(this, &MainWindow::connectToPeerRequested, networkManager, &NetworkManager::connectToPeer);
    connect(this, &MainWindow::disconnectFromPeerRequested, networkManager, &NetworkManager::disconnectFromPeer);
    connect(this, &MainWindow::startAudioStreamingRequested, networkManager, &NetworkManager::startAudioStreaming);
    connect(this, &MainWindow::stopAudioStreamingRequested, networkManager, &NetworkManager::stopAudioStreaming);

    // Connect the network manager signals to the UI slots
    connect(networkManager, &NetworkManager::peerDiscovered, this, &MainWindow::handlePeerDiscovered);
    connect(networkManager, &NetworkManager::connectionStatusUpdated, this, &MainWindow::handleConnectionStatusUpdated);
}

MainWindow::~MainWindow() {}

void MainWindow::startDiscovery()
{
    emit startDiscoveryRequested();
}

void MainWindow::stopDiscovery()
{
    emit stopDiscoveryRequested();
}

void MainWindow::connectToPeer()
{
    QListWidgetItem* selectedPeerItem = peerListWidget->currentItem();
    if (selectedPeerItem) {
        QString address = selectedPeerItem->data(Qt::UserRole).toString();
        emit connectToPeerRequested(address);
    }
}

void MainWindow::disconnectFromPeer()
{
    QListWidgetItem* selectedPeerItem = peerListWidget->currentItem();
    if (selectedPeerItem) {
        QString address = selectedPeerItem->data(Qt::UserRole).toString();
        emit disconnectFromPeerRequested(address);
    }
}

void MainWindow::startAudioStreaming()
{
    QListWidgetItem* selectedPeerItem = peerListWidget->currentItem();
    if (selectedPeerItem) {
        QString address = selectedPeerItem->data(Qt::UserRole).toString();
        emit startAudioStreamingRequested(address);
    }
}

void MainWindow::stopAudioStreaming()
{
    QListWidgetItem* selectedPeerItem = peerListWidget->currentItem();
    if (selectedPeerItem) {
        QString address = selectedPeerItem->data(Qt::UserRole).toString();
        emit stopAudioStreamingRequested(address);
    }
}

void MainWindow::handlePeerDiscovered(QString name, QString address)
{
    QListWidgetItem* peerItem = new QListWidgetItem(name);
    peerItem->setData(Qt::UserRole, address);
    peerListWidget->addItem(peerItem);
}

void MainWindow::handleConnectionStatusUpdated(int index, bool connected)
{
    QListWidgetItem* peerItem = peerListWidget->item(index);
    if (peerItem) {
        QString address = peerItem->data(Qt::UserRole).toString();
        if (connected) {
            peerItem->setText(address + " (Connected)");
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(true);
            startStreamingButton->setEnabled(true);
            stopStreamingButton->setEnabled(false);
        } else {
            peerItem->setText(address + " (Disconnected)");
            connectButton->setEnabled(true);
            disconnectButton->setEnabled(false);
            startStreamingButton->setEnabled(false);
            stopStreamingButton->setEnabled(false);
        }
    }
}

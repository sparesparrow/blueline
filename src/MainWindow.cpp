// MainWindow.cpp

#include "MainWindow.h"
#include <QLabel>
#include <QSpacerItem>
#include <QGroupBox>
#include <QIcon>

MainWindow::MainWindow(NetworkManager* networkManager, QWidget* parent)
    : QMainWindow(parent)
    , networkManager(networkManager)
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    QLabel* peersLabel = new QLabel("Peers:", this);
    layout->addWidget(peersLabel);

    peerListWidget = new QListWidget(this);
    layout->addWidget(peerListWidget);

    QGroupBox* connectionBox = new QGroupBox("Connection", this);
    QVBoxLayout* connectionLayout = new QVBoxLayout(connectionBox);

    connectButton = new QPushButton(QIcon(":/resources/connect.png"), "Connect", this);
    connectionLayout->addWidget(connectButton);

    disconnectButton = new QPushButton(QIcon(":/resources/disconnect.png"), "Disconnect", this);
    connectionLayout->addWidget(disconnectButton);

    layout->addWidget(connectionBox);

    QGroupBox* streamingBox = new QGroupBox("Streaming", this);
    QVBoxLayout* streamingLayout = new QVBoxLayout(streamingBox);

    startStreamingButton = new QPushButton(QIcon(":/resources/play.png"), "Start Streaming", this);
    streamingLayout->addWidget(startStreamingButton);

    stopStreamingButton = new QPushButton(QIcon(":/resources/stop.png"), "Stop Streaming", this);
    streamingLayout->addWidget(stopStreamingButton);

    layout->addWidget(streamingBox);

    // Add a spacer
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Add a status label
    QLabel* statusLabel = new QLabel("Status: Not connected", this);
    layout->addWidget(statusLabel);

    // Connect the rest of your signals and slots as before...
}

#!/bin/bash

# Define the include directives for each file
declare -A includes=(
  ["AudioService.h"]="#include <QObject>\n#include <QByteArray>"
  ["AudioService.cpp"]="#include \"AudioService.h\"\n#include <QBuffer>\n#include <QDebug>"
  ["AudioPlayer.h"]="#include \"AudioService.h\"\n#include <QAudioOutput>\n#include <QBuffer>"
  ["AudioPlayer.cpp"]="#include \"AudioPlayer.h\"\n#include <QDebug>"
  ["AudioStreamer.h"]="#include \"AudioService.h\"\n#include <QDataStream>"
  ["AudioStreamer.cpp"]="#include \"AudioStreamer.h\"\n#include <QDebug>"
  ["InterfaceSocket.h"]="#include <QObject>\n#include <QSharedPointer>\n#include <QUdpSocket>\n#include <QHostAddress>\n#include <QNetworkInterface>"
  ["InterfaceSocketServer.h"]="#include \"InterfaceSocket.h\""
  ["InterfaceSocketClient.h"]="#include \"InterfaceSocket.h\""
  ["AudioServiceFactory.h"]="#include \"InterfaceSocketServer.h\"\n#include \"InterfaceSocketClient.h\""
  ["NetworkPeer.h"]="#include <QObject>\n#include <QSharedPointer>\n#include \"InterfaceSocket.h\"\n#include \"AudioService.h\""
  ["NetworkPeer.cpp"]="#include \"NetworkPeer.h\"\n#include <QDebug>"
  ["NetworkManager.h"]="#include <QObject>\n#include <QSharedPointer>\n#include <QList>\n#include \"NetworkPeer.h\"\n#include \"InterfaceSocket.h\""
  ["NetworkManager.cpp"]="#include \"NetworkManager.h\"\n#include <QDebug>"
  ["MainWindow.h"]="#include <QMainWindow>\n#include <QListWidget>\n#include <QPushButton>\n#include <QVBoxLayout>\n#include \"NetworkManager.h\""
  ["MainWindow.cpp"]="#include \"MainWindow.h\"\n#include <QDebug>"
  ["main.cpp"]="#include <QApplication>\n#include \"MainWindow.h\"\n#include \"NetworkManager.h\""
)

# Loop through each file and modify the include directives
for file in "${!includes[@]}"; do
  if [[ -f "$file" ]]; then
    echo "Modifying $file"
    temp_file=".temp_$file"
    sed -e "1s|^|${includes[$file]}\n|" "$file" >"$temp_file"
    mv "$temp_file" "$file"
  else
    echo "File $file not found!"
  fi
done

echo "Include directives modification complete!"

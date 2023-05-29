import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    title: "Bluetooth Manager"
    width: 400
    height: 300
    visible: true

    ListView {
        id: deviceListView
        width: parent.width
        height: parent.height - bindAudioButton.height
        model: userInterface.deviceModel // A QML property exposed from UserInterface class
        delegate: Item {
            width: parent.width
            height: 50

            Rectangle {
                width: parent.width
                height: parent.height
                color: {
                    // Change the color based on the device status
                    switch (model.status) {
                        case 0: // ConnectionStatus.DISCONNECTED
                            return "red";
                        case 1: // ConnectionStatus.CONNECTED
                            return "green";
                        case 2: // ConnectionStatus.IN_USE
                            return "orange";
                    }
                }
                Text {
                    text: model.name
                    anchors.centerIn: parent
                }
            }
        }
    }

    Button {
        id: bindAudioButton
        text: "Bind Audio"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: deviceListView.currentIndex !== -1
        onClicked: userInterface.bindAudio(deviceListView.currentIndex)
    }
}

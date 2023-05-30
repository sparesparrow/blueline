// DeviceItem.qml
import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    width: parent.width
    height: 30

    property string deviceId
    property string deviceName

    Rectangle {
        width: parent.width
        height: parent.height
        color: "transparent"
        border.color: "black"
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: deviceName
        }
    }
}

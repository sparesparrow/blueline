// DeviceListView.qml
import QtQuick 2.12
import QtQuick.Controls 2.12

ListView {
    id: deviceListView
    width: parent.width
    height: parent.height - bindAudioButton.height
    model: userInterface.deviceModel

    delegate: DeviceItem {
        deviceId: model.deviceId
        deviceName: model.deviceName
    }
}

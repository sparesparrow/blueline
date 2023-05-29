// MainView.qml
import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    title: "Bluetooth Manager"
    width: 400
    height: 300
    visible: true

    DeviceListView {
        id: deviceListView
    }

    BindAudioButton {
        id: bindAudioButton
    }

    BindAudioDialog {
        id: bindAudioDialog
    }
}

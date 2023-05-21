// BindAudioDialog.qml
import QtQuick 2.12
import QtQuick.Controls 2.12

Dialog {
    id: bindAudioDialog
    title: "Bind Audio"
    standardButtons: StandardButton.Ok | StandardButton.Cancel

    Column {
        spacing: 10
        width: 300

        ComboBox {
            id: inputComboBox
            width: parent.width

            model: userInterface.getInputDevices()
            textRole: "deviceName"
            displayText: "Select Input Device"
        }

        ComboBox {
            id: outputComboBox
            width: parent.width

            model: userInterface.getOutputDevices()
            textRole: "deviceName"
            displayText: "Select Output Device"
        }
    }

    onAccepted: {
        userInterface.bindAudio(inputComboBox.currentItem, outputComboBox.currentItem)
    }
}

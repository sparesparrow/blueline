// BindAudioButton.qml
import QtQuick 2.12
import QtQuick.Controls 2.12

Button {
    id: bindAudioButton
    text: "Bind Audio"
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    onClicked: bindAudioDialog.open()
}

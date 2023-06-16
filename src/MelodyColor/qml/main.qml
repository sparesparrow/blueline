import QtQuick 2.0
import MelodyColor 1.0
import QtMultimedia 5.15
import QtMultimedia 6.6
import QtQuick.Window 2.3

import "."

Window {
    title: "MelodyColor"
    width: 400
    height: 300
    visible: true
    id: melodyColor

    AudioService {
        id: audioService
        // Configurations for audioService
    }

    AudioColorProvider {
        id: audioColorProvider
        // Configurations for audioColorProvider
    }

    Rectangle {
        id: screen
        width: 640
        height: 480
        color: "#F5F5F5"

        function colorForSemitone(semitone) { // Define color mapping function as a separate function
            var hue = (semitone % 12) * 30 // Map semitone to hue value (0-360)
            return Qt.hsla(hue, 1, 0.5, 1) // Return color based on hue value
        }

        Text {
            id: title
            text: "Spectrum Analyzer"
            font.pixelSize: 24
            font.bold: true
            color: "#333333"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
        }

        Text {
            id: noteText
            text: "Note: "  // If you want to display notes, connect this to the appropriate data source
            font.pixelSize: 18
            color: "#333333"
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
        }

        Text {
            id: semitoneText
            text: "Semitone: "  // Initial value
            font.pixelSize: 18
            color: "#333333"
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
        }
    }

    Timer {
        id: timer
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            // Call method to read data from audio service and pass it to audio color provider
        }
    }
}

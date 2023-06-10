import QtQuick 2.15
import QtQuick.Window 2.15
import QtMultimedia 5.15

Window {
    visible: true
    width: 640
    height: 480
    title: "Audio Visualizer"

    Rectangle {
        id: colorBox
        anchors.fill: parent
    }

    AudioRecorder {
        id: audioRecorder
        audioInput: "default"

        AudioEncoderSettings {
            codec: "audio/pcm"
            sampleRate: 44100
            channelCount: 2
            bitRate: 128000
            quality: HighQuality
            encodingMode: ConstantQualityEncoding
        }

        onStateChanged: {
            if (audioRecorder.state === AudioRecorder.RecordingState) {
                spectrumAnalyser.start()
            } else {
                spectrumAnalyser.stop()
            }
        }
    }

    SpectrumAnalyser {
        id: spectrumAnalyser
        audioRecorder: audioRecorder
        Connections {
            target: spectrumAnalyser
            onSemitoneChanged: {
                var note = semitone[0];
                switch (note) {
                    case "C":
                        colorBox.color = "red";
                        break;
                    case "C#":
                        colorBox.color = "orange";
                        break;
                    case "D":
                        colorBox.color = "yellow";
                        break;
                    case "D#":
                        colorBox.color = "lime";
                        break;
                    case "E":
                        colorBox.color = "green";
                        break;
                    case "F":
                        colorBox.color = "cyan";
                        break;
                    case "F#":
                        colorBox.color = "lightblue";
                        break;
                    case "G":
                        colorBox.color = "blue";
                        break;
                    case "G#":
                        colorBox.color = "purple";
                        break;
                    case "A":
                        colorBox.color = "magenta";
                        break;
                    case "A#":
                        colorBox.color = "pink";
                        break;
                    case "B":
                        colorBox.color = "white";
                        break;
                    default:
                        colorBox.color = "black";
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (audioRecorder.state === AudioRecorder.StoppedState) {
                audioRecorder.record()
            } else {
                audioRecorder.stop()
            }
        }
    }
    function drawVisualization(semitone) {
    var context = canvas.getContext('2d');
    context.clearRect(0, 0, canvas.width, canvas.height);
    var color;
    switch (semitone[0]) {
        case "C":
            color = "red";
            break;
        case "C#":
            color = "orange";
            break;
        case "D":
            color = "yellow";
            break;
        case "D#":
            color = "lime";
            break;
        case "E":
            color = "green";
            break;
        case "F":
            color = "cyan";
            break;
        case "F#":
            color = "lightblue";
            break;
        case "G":
            color = "blue";
            break;
        case "G#":
            color = "purple";
            break;
        case "A":
            color = "magenta";
            break;
        case "A#":
            color = "pink";
            break;
        case "B":
            color = "white";
            break;
        default:
            color = "black";
    }
    context.fillStyle = color;
    context.fillRect(0, 0, canvas.width, canvas.height);
}

}

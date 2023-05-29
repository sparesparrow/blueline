#include "AudioIOManager.h"
#include "BluetoothManager.h"
#include <QDebug>

AudioIOManager::AudioIOManager(QObject *parent)
    : QObject(parent), audioInput(nullptr), audioOutput(nullptr) {}

AudioIOManager::~AudioIOManager() {
    delete audioInput;
    delete audioOutput;
}

void AudioIOManager::bindInput(const Device& device) {
    // Set up the audio format for input
    QAudioFormat format;
    // Set up the desired format, for example:
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::UInt8);

    QAudioDevice info = QMediaDevices::defaultAudioInput();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.preferredFormat();
    }

    delete audioInput;
    audioInput = new QAudioSource(format, this);
    connect(audioInput, &QAudioSource::stateChanged, this, &AudioIOManager::handleStateChangedInput);

    // Set the input device for audio processing
    // ...

    // Emit the input device changed signal
    // ...
}

void AudioIOManager::bindOutput(const Device& device) {
    // Set up the audio format for output
    QAudioFormat format;
    // Set up the desired format, for example:
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::UInt8);

    QAudioDevice info = QMediaDevices::defaultAudioOutput();
    if (!info.isFormatSupported(format)) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.preferredFormat();
    }

    delete audioOutput;
    audioOutput = new QAudioSink(format, this);
    connect(audioOutput, &QAudioSink::stateChanged, this, &AudioIOManager::handleStateChangedOutput);

    // Set the output device for audio processing
    // ...

    // Emit the output device changed signal
    // ...
}

void AudioIOManager::handleStateChangedInput(QAudio::State newState) {
    switch (newState) {
    case QAudio::IdleState:
        // Finished recording
        break;
    case QAudio::StoppedState:
        // Stopped for other reasons
        break;
    default:
        // ... other cases as appropriate
        break;
    }
}

void AudioIOManager::handleStateChangedOutput(QAudio::State newState) {
    switch (newState) {
    case QAudio::IdleState:
        // Finished playing (no more data)
        break;
    case QAudio::StoppedState:
        // Stopped for other reasons
        break;
    default:
        // ... other cases as appropriate
        break;
    }
}

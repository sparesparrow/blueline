// AudioIOManager.cpp
#include "AudioIOManager.h"
#include <QDebug>

AudioIOManager::AudioIOManager(QObject *parent)
    : QObject(parent), audioInput(nullptr), audioOutput(nullptr) {}

AudioIOManager::~AudioIOManager() {
    delete audioInput;
    delete audioOutput;
}

void AudioIOManager::bindInput(const Device& device) {
    // Ideally, Device object should provide QAudioDeviceInfo as well
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(audioInput->format())) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        audioInput->setFormat(info.nearestFormat(audioInput->format()));
    }

    delete audioInput; // Deleting the old QAudioInput before creating a new one
    audioInput = new QAudioInput(info, audioInput->format(), this);
    connect(audioInput, &QAudioInput::stateChanged, this, &AudioIOManager::handleStateChangedInput);

    emit inputDeviceChanged(device);
}

void AudioIOManager::bindOutput(const Device& device) {
    // Ideally, Device object should provide QAudioDeviceInfo as well
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(audioOutput->format())) {
        qWarning() << "Default format not supported, trying to use the nearest.";
        audioOutput->setFormat(info.nearestFormat(audioOutput->format()));
    }

    delete audioOutput; // Deleting the old QAudioOutput before creating a new one
    audioOutput = new QAudioOutput(info, audioOutput->format(), this);
    connect(audioOutput, &QAudioOutput::stateChanged, this, &AudioIOManager::handleStateChangedOutput);

    emit outputDeviceChanged(device);
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

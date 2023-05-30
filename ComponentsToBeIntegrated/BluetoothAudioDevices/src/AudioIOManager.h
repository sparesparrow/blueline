#ifndef AUDIOIOMANAGER_H
#define AUDIOIOMANAGER_H

#include <QObject>
#include <QAudioSource>
#include <QAudioSink>
#include <QMediaDevices>
#include <QMediaFormat>
#include "Device.h"

class AudioIOManager : public QObject {
    Q_OBJECT

public:
    AudioIOManager(QObject *parent = nullptr);
    ~AudioIOManager();
    void bindInput(const Device& device);
    void bindOutput(const Device& device);

private slots:
    void handleStateChangedInput(QAudio::State newState);
    void handleStateChangedOutput(QAudio::State newState);

private:
    QAudioSource* audioInput;
    QAudioSink* audioOutput;
};

#endif // AUDIOIOMANAGER_H

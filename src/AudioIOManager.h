// AudioIOManager.h
#ifndef AUDIOIOMANAGER_H
#define AUDIOIOMANAGER_H

#include <QAudioInput>
#include <QAudioOutput>
#include <QObject>
#include "Device.h"

class AudioIOManager : public QObject {
    Q_OBJECT

public:
    explicit AudioIOManager(QObject *parent = nullptr);
    ~AudioIOManager();
    void bindInput(const Device& device);
    void bindOutput(const Device& device);

signals:
    void inputDeviceChanged(const Device& device);
    void outputDeviceChanged(const Device& device);

private slots:
    void handleStateChangedInput(QAudio::State newState);
    void handleStateChangedOutput(QAudio::State newState);

private:
    QAudioInput *audioInput;
    QAudioOutput *audioOutput;
};

#endif // AUDIOIOMANAGER_H

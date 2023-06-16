// AudioService.h
#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <QObject>
#include <QByteArray>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QAbstractSocket>
#include <QBuffer>
#include <QAudioBuffer>
#include <QtMultimedia>
#include <QSharedPointer>
#include <QIODevice>
#include <memory>
#include "AudioPlayer.h"
#include "AudioStreamer.h"
#include "AudioCapture.h"
#include "AudioProcessor.h"
#include "SpectrumVisualizer.h"

// Segregated interfaces from IAudioHandler
class IAudioDataReceiver {
public:
    virtual ~IAudioDataReceiver() = default;
    virtual void handleAudioData(const QByteArray& audioData) = 0;
};

class IAudioDataEmitter
{
public:
    virtual ~IAudioDataEmitter() = default;
    virtual void emitAudioData(const QByteArray& audioData) = 0;
};

class AudioDataHandler : public QObject
{
    Q_OBJECT
public:
    explicit AudioDataHandler(std::shared_ptr<AudioProcessor> audioProcessor, QObject* parent = nullptr)
        : QObject(parent), audioProcessor(audioProcessor)
    {
    }
    virtual ~AudioDataHandler() = default;

signals:
    void audioDataReady(const QByteArray& audioData);

public slots:
    void handleAudioData(const QByteArray& audioData) {
        if (audioData.isEmpty()) {
            return;
        }
        const char* dataPtr = audioData.constData();
        const auto dataSize = audioData.size();
        audioProcessor->processBuffer(dataPtr, dataSize);
        emit audioDataReady(audioData);
    }

private:
    std::shared_ptr<AudioProcessor> audioProcessor;
};

class AudioStreamer : public QObject
{
    Q_OBJECT
public:
    explicit AudioStreamer(QSharedPointer<IAudioCapture> audioCapture, QSharedPointer<IAudioPlayer> audioPlayer, QObject* parent = nullptr)
        : QObject(parent), audioCapture(audioCapture), audioPlayer(audioPlayer)
    {
        connect(audioCapture.data(), &IAudioCapture::audioDataProvided, this, &AudioStreamer::handleAudioDataProvided);
        connect(this, &AudioStreamer::audioPlayRequested, audioPlayer.data(), &IAudioPlayer::play);
        connect(audioDataHandler.data(), &AudioDataHandler::audioDataReady, this, &AudioStreamer::handleAudioDataProvided);
    }

public slots:
    void handleAudioDataProvided(const QByteArray& audioData) {
        audioCapture->readData();
        emit audioDataRequested(audioData);
    }

signals:
    void audioDataRequested(const QByteArray& audioData);
    void audioBufferRequested(QSharedPointer<QAudioBuffer> audioBufferPtr);
    void audioPlayRequested();

private:
    QSharedPointer<IAudioCapture> audioCapture;
    QSharedPointer<IAudioPlayer> audioPlayer;
    QSharedPointer<AudioDataHandler> audioDataHandler;
};

class AudioService: public QObject, public IAudioDataReceiver, public IAudioDataEmitter
{
    Q_OBJECT
public:
    explicit AudioService(QSharedPointer<IAudioPlayer> player, QSharedPointer<IAudioCapture> capture, QObject* parent = nullptr)
        : QObject(parent), audioPlayer(player), audioCapture(capture)
    {
        connect(audioCapture.data(), &IAudioCapture::audioDataProvided, this, &AudioService::handleAudioDataProvided);
    }

    QSharedPointer<IAudioPlayer> getAudioPlayer() const {
        return audioPlayer;
    }

    QSharedPointer<IAudioCapture> getAudioCapture() const {
        return audioCapture;
    }

public slots:
    void handleAudioDataProvided(const QByteArray& audioData) {
        audioCapture->readData();
        emit audioDataRequested(audioData);
    }

signals:
    void audioDataRequested(const QByteArray& audioData);

private:
    QSharedPointer<IAudioPlayer> audioPlayer;
    QSharedPointer<IAudioCapture> audioCapture;
};


class VisualizerUpdater : public QObject
{
    Q_OBJECT
public:
    explicit VisualizerUpdater(VisualizerQml* visualizer, std::shared_ptr<AudioProcessor> audioProcessor, QObject* parent = nullptr)
        : QObject(parent), visualizer(visualizer), audioProcessor(audioProcessor)
    {
        connect(this, &VisualizerUpdater::semitoneChanged, visualizer, &VisualizerQml::updateVisualization);
    }

signals:
    void semitoneChanged(QString note);

public slots:
    void handleAudioData(const QByteArray& audioData) {
        if (audioData.isEmpty()) {
            return;
        }
        const char* dataPtr = audioData.constData();
        const auto dataSize = audioData.size();
        const auto semitone = audioProcessor->processBuffer(dataPtr, dataSize);

        // Emit the semitone changed signal
        QString note = QString::number(semitone, 'f', 2);
        emit semitoneChanged(note);
    }

private:
    VisualizerQml* visualizer;
    std::shared_ptr<AudioProcessor> audioProcessor;
};

class AudioColorProvider : public QObject, public IAudioDataReceiver
{
    Q_OBJECT
public:
    AudioColorProvider(VisualizerQml* visualizer, QObject* parent = nullptr)
        : QObject(parent)
    {
        auto audioProcessor = std::make_shared<AudioProcessor>();
        audioDataHandler = std::make_shared<AudioDataHandler>(audioProcessor);
        visualizerUpdater = std::make_shared<VisualizerUpdater>(visualizer, audioProcessor);

        connect(audioDataHandler.get(), &AudioDataHandler::audioDataReady, visualizerUpdater.get(), &VisualizerUpdater::handleAudioData);
        connect(visualizerUpdater.get(), &VisualizerUpdater::semitoneChanged, this, &AudioColorProvider::semitoneChanged);
    }

    void handleAudioData(const QByteArray& audioData) override {
        audioDataHandler->handleAudioData(audioData);
    }

signals:
    void semitoneChanged(QString note);
    void colorChanged(QString color);
    void audioDataReady(const QByteArray& audioData);

public slots:
    void readAudioData(QIODevice *device)
    {
        if (!device) {
            return;
        }
        const auto audioData = device->readAll();
        if (audioData.isEmpty()) {
            return;
        }
        handleAudioData(audioData);
        emit audioDataReady(audioData);
    }

private:
    std::shared_ptr<AudioDataHandler> audioDataHandler;
    std::shared_ptr<VisualizerUpdater> visualizerUpdater;
    std::shared_ptr<AudioProcessor> audioProcessor;
};

#endif // AUDIOSERVICE_H

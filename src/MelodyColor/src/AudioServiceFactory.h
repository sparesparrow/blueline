// AudioServiceFactory.h
#ifndef AUDIOSERVICEFACTORY_H
#define AUDIOSERVICEFACTORY_H

#include <memory>
#include <QAudioOutput>
#include <QAudioSink>
#include <QAbstractSocket>
#include <QAudioFormat>
#include <QHostAddress>
#include <QUdpSocket>
#include "AudioService.h"
#include "AudioPlayer.h"
#include "AudioStreamer.h"
#include "AudioCapture.h"

// This file contains factory classes for creating various audio-related objects.
/*
class AudioDeviceFactory
{
public:
    std::shared_ptr<QObject> createAudioDevice(MessageType messageType, QIODevice* device = nullptr, QAudioFormat format = QAudioFormat())
    {
        switch (messageType)
        {
        case MessageType::StartAudioStreamRequest:
            return std::make_shared<AudioCapture>(format);
        case MessageType::StartAudioStreamResponse:
            return std::make_shared<AudioPlayer>(device, format);
        default:
            throw std::invalid_argument("Invalid message type for audio device creation");
        }
    }
};
*/

class IAudioPlayerFactory {
public:
    virtual ~IAudioPlayerFactory() = default;
    virtual QSharedPointer<IAudioPlayer> create(QSharedPointer<QIODevice> sourceDevice) = 0;
};

class IAudioCaptureFactory {
public:
    virtual ~IAudioCaptureFactory() = default;
    virtual QSharedPointer<IAudioCapture> create() = 0;
};


// Factory for creating an AudioPlayer object
// Takes in a QAudioFormat object and a QObject parent
class AudioPlayerFactory : public IAudioPlayerFactory {
public:
    explicit AudioPlayerFactory(QAudioFormat format, QObject* parent = nullptr)
        : format(format), parent(parent)
    {}

    virtual ~AudioPlayerFactory() = default;

    // Implementation of the create method that creates an AudioPlayer object
    // using the given QIODevice object, QAudioFormat object, and QObject parent
    QSharedPointer<IAudioPlayer> create(QSharedPointer<QIODevice> sourceDevice) override
    {
        return QSharedPointer<AudioPlayer>::create(sourceDevice, format, parent);
    }

private:
    QAudioFormat format;
    QObject* parent;
};

// Factory for creating a MediaAudioPlayer object
// Takes in required arguments for creating a MediaAudioPlayer object
class MediaAudioPlayerFactory : public IAudioPlayerFactory {
public:
    explicit MediaAudioPlayerFactory(/* Your arguments here */) {
        // initialize your members here
    }
    virtual ~MediaAudioPlayerFactory() = default;

    // Implementation of the create method that creates a MediaAudioPlayer object
    QSharedPointer<IAudioPlayer> create() override {
        // Your implementation here
    }
};
// Factory for creating an AudioCapture object
// Takes in a QAudioFormat object
class AudioCaptureFactory : public IAudioCaptureFactory {
public:
    explicit AudioCaptureFactory(QAudioFormat format)
        : format(format)
    {}
    virtual ~AudioCaptureFactory() = default;

    // Implementation of the create method that creates an AudioCapture object
    QSharedPointer<IAudioCapture> create() override
    {
        return QSharedPointer<AudioCapture>::create(format);
    }

private:
    QAudioFormat format;
};

// Factory for creating an AudioRecorder object
// Takes in required arguments for creating an AudioRecorder object
class AudioRecorderFactory : public IAudioCaptureFactory {
public:
    explicit AudioRecorderFactory(/* Your arguments here */) {
        // initialize your members here
    }
    virtual ~AudioRecorderFactory() = default;

    // Implementation of the create method that creates an AudioRecorder object
    QSharedPointer<IAudioCapture> create() override {
        // Your implementation here
    }
};

class IAudioHandlerFactory {
public:
    virtual ~IAudioHandlerFactory() = default;
    virtual QSharedPointer<IAudioHandler> create() = 0;
};


class MelodyColorControllerFactory : public IAudioHandlerFactory {
public:
    explicit MelodyColorControllerFactory(std::unique_ptr<AudioRecorder> audioRecorder, VisualizerQml* visualizer)
        : audioRecorder(std::move(audioRecorder)), visualizer(visualizer)
    {}
    QSharedPointer<IAudioHandler> create() override
    {
        auto melodyColorController = QSharedPointer<AudioColorProvider>::create(std::move(audioRecorder), visualizer);
        return melodyColorController;
    }
private:
    std::unique_ptr<AudioRecorder> audioRecorder;
    VisualizerQml* visualizer;
};

class AudioServiceFactory : public IAudioHandlerFactory {
public:
    explicit AudioServiceFactory(QSharedPointer<IAudioPlayer> player, QSharedPointer<IAudioCapture> capture)
        : audioPlayer(player), audioCapture(capture)
    {}

    QSharedPointer<IAudioHandler> create() override
    {
        auto audioService = QSharedPointer<AudioService>::create(audioPlayer, audioCapture);
        connect(audioCapture.data(), &IAudioCapture::audioDataProvided, audioService.data(), &AudioService::handleAudioDataProvided);
        return audioService;
    }

private:
    QSharedPointer<IAudioPlayer> audioPlayer;
    QSharedPointer<IAudioCapture> audioCapture;
};

#endif // AUDIOSERVICEFACTORY_H

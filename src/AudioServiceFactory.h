// AudioServiceFactory.h
#ifndef AUDIOSERVICEFACTORY_H
#define AUDIOSERVICEFACTORY_H

#include <QAudioOutput>
#include <QAudioSink>
#include <QAbstractSocket>
#include <QAudioFormat>
#include <QHostAddress>
#include <QUdpSocket>
#include <memory>
#include "AudioService.h"
#include "Message.h"

class AudioPlayerFactory {
public:
    explicit AudioPlayerFactory(QAudioFormat format, QObject* parent = nullptr)
        : format(format), parent(parent)
    {}
    virtual ~AudioPlayerFactory() = default;
    std::unique_ptr<AudioPlayer> create(QSharedPointer<QIODevice> sourceDevice)
    {
        return std::make_unique<AudioPlayer>(sourceDevice, format, parent);
    }
private:
    QAudioFormat format;
    QObject* parent;
};

class AudioStreamerFactory {
public:
    explicit AudioStreamerFactory(QHostAddress multicastGroupAddress, quint16 multicastPort, QObject* parent = nullptr)
        : multicastGroupAddress(multicastGroupAddress), multicastPort(multicastPort), parent(parent)
    {}
    virtual ~AudioStreamerFactory() = default;
    std::unique_ptr<AudioStreamer> create(QSharedPointer<QAbstractSocket> socket)
    {
        return std::make_unique<AudioStreamer>(socket, parent);
    }
private:
    QHostAddress multicastGroupAddress;
    quint16 multicastPort;
    QObject* parent;
};

class AudioServiceFactory {
public:
    explicit AudioServiceFactory(MessageType type, QAudioFormat format, QHostAddress multicastGroupAddress, quint16 multicastPort, QObject* parent = nullptr)
        : type(type), playerFactory(format, parent), streamerFactory(multicastGroupAddress, multicastPort, parent), parent(parent)
    {}
    virtual ~AudioServiceFactory() = default;
    std::unique_ptr<AudioService> create()
    {
        return std::make_unique<AudioService>(playerFactory.create(), streamerFactory.create());
    }
    QSharedPointer<AudioService> createAudioService(quint16 port) {
        return QSharedPointer<AudioService>(new AudioService(type, port));
    }
private:
    MessageType type;
    AudioPlayerFactory playerFactory;
    AudioStreamerFactory streamerFactory;
    QObject* parent;
};

#endif // AUDIOSERVICEFACTORY_H

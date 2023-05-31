
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

/**
 * @brief Factory class for creating audio players.
 */
class AudioPlayerFactory {
public:
    explicit AudioPlayerFactory(QAudioFormat format, QObject* parent = nullptr)
        : format(format), parent(parent)
    {}
    virtual ~AudioPlayerFactory() = default;

    std::unique_ptr<AudioPlayer> create()
    {
        return std::make_unique<AudioPlayer>(std::make_unique<QAudioSink>(format, parent));
    }
private:
    QAudioFormat format;
    QObject* parent;
};

/**
 * @brief Factory class for creating audio streamers.
 */
class AudioStreamerFactory {
public:
    explicit AudioStreamerFactory(QHostAddress multicastGroupAddress, quint16 multicastPort, QObject* parent = nullptr)
        : multicastGroupAddress(multicastGroupAddress), multicastPort(multicastPort), parent(parent)
    {}
    virtual ~AudioStreamerFactory() = default;

    std::unique_ptr<AudioStreamer> create()
    {
        auto socket = std::make_unique<QUdpSocket>(parent);
        socket->bind(multicastGroupAddress, multicastPort, QAbstractSocket::ShareAddress);
        return std::make_unique<AudioStreamer>(std::move(socket));
    }
private:
    QHostAddress multicastGroupAddress;
    quint16 multicastPort;
    QObject* parent;
};

/**
 * @brief Factory class for creating audio services.
 */
class AudioServiceFactory {
public:
    explicit AudioServiceFactory(QAudioFormat format, QHostAddress multicastGroupAddress, quint16 multicastPort, QObject* parent = nullptr)
        : playerFactory(format, parent), streamerFactory(multicastGroupAddress, multicastPort, parent), parent(parent)
    {}
    virtual ~AudioServiceFactory() = default;

    std::unique_ptr<AudioService> create()
    {
        return std::make_unique<AudioService>(playerFactory.create(), streamerFactory.create());
    }
private:
    AudioPlayerFactory playerFactory;
    AudioStreamerFactory streamerFactory;
    QObject* parent;
};

#endif // AUDIOSERVICEFACTORY_H

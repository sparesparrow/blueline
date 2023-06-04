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
        : multicastGroupAddress(multicastGroupAddress), multicastPort(multicastPort)
    {}

    virtual ~AudioStreamerFactory() = default;

    std::unique_ptr<AudioStreamer> create() {
        QSharedPointer<QUdpSocket> socket = QSharedPointer<QUdpSocket>::create();
        socket->setSocketOption(QAbstractSocket::MulticastTtlOption, 1); // Set TTL to 1 for local network
        socket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, true); // Enable loopback for testing
        socket->bind(QHostAddress::AnyIPv4, multicastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        socket->joinMulticastGroup(multicastGroupAddress);
        return std::make_unique<AudioStreamer>(socket);
    }

private:
    QHostAddress multicastGroupAddress;
    quint16 multicastPort;
};

class AudioCaptureFactory {
public:
    explicit AudioCaptureFactory(QAudioFormat format)
        : format(format)
    {}

    std::unique_ptr<AudioCapture> create()
    {
        return std::make_unique<AudioCapture>(format);
    }

private:
    QAudioFormat format;
};

class AudioServiceFactory {
public:
    explicit AudioServiceFactory(QSharedPointer<AudioPlayer> player, QSharedPointer<AudioStreamer> streamer, QObject* parent = nullptr)
        : player(player), streamer(streamer), parent(parent)
    {}

    std::unique_ptr<AudioService> create() {
        return std::make_unique<AudioService>(player, streamer);
    }

private:
    QSharedPointer<AudioPlayer> player;
    QSharedPointer<AudioStreamer> streamer;
    QObject* parent;
};
/* TODO: review factory reimplemenation
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
*/

#endif // AUDIOSERVICEFACTORY_H

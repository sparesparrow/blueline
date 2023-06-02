// AudioService.h
#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <QObject>
#include <QByteArray>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDevice>
#include <QAbstractSocket>
#include <QBuffer>
#include <QAudioBuffer>
#include <QtMultimedia>
#include <QSharedPointer>
#include <QIODevice>
#include <memory>
#include "Message.h"

/**
 * @brief AudioPlayer is responsible for playing audio data from remote audio source.
 */
class AudioPlayer : public QObject {
    Q_OBJECT
public:
    explicit AudioPlayer(QSharedPointer<QIODevice> sourceDevice, const QAudioFormat& audioFormat, QObject* parent = nullptr)
        : QObject(parent), sourceDevice(sourceDevice), audioFormat(audioFormat), audioDecoder(this)
    {
        audioDecoder.setSourceDevice(sourceDevice.data());
        connect(&audioDecoder, &QAudioDecoder::bufferReady, this, &AudioPlayer::readAudioData);

        connect(&audioDecoder, SIGNAL(bufferReady()), this, SLOT(readAudioData()));
        connect(sourceDevice.data(), SIGNAL(readyRead()), this, SLOT(readAudioData()));
        connect(this, SIGNAL(audioDataRequested(QSharedPointer<QIODevice>)), this, SLOT(provideAudioData(QSharedPointer<QIODevice>)));
        connect(this, SIGNAL(audioBufferRead(QSharedPointer<QAudioBuffer>)), this, SLOT(provideAudioData(QSharedPointer<QAudioBuffer>)));
        connect(sourceDevice.data(), SIGNAL(readyRead()), this, SLOT(playAudioData()));
        audioDecoder.start();
    }
    QSharedPointer<QAudioDecoder> getAudioDecoder() {
        return QSharedPointer<QAudioDecoder>(&audioDecoder);
    }
signals:
    void audioDataRequested(QSharedPointer<QIODevice> targetDevice);
    void audioBufferRead(QSharedPointer<QAudioBuffer> targetDevice);
    void audioDataProvided(QAudioDevice targetDevice);
public slots:
    void readAudioData()
    {
        QAudioBuffer audioBuffer = audioDecoder.read();
        QSharedPointer<QAudioBuffer> audioBufferPtr = QSharedPointer<QAudioBuffer>(&audioBuffer);
        emit audioBufferRead(audioBufferPtr);
    }
    void provideAudioData(QSharedPointer<QIODevice> targetDevice)
    {
        audioDecoder.start();
        emit audioDataRequested(targetDevice);
    }
    void playAudioData(QSharedPointer<QAudioBuffer> audioBufferPtr)
    {
        QMediaPlayer* mediaPlayer = new QMediaPlayer;
        QAudioOutput* audioOutput = new QAudioOutput;
        audioBufferPtr.data();
        mediaPlayer->setSourceDevice(sourceDevice.data());
        mediaPlayer->setAudioOutput(audioOutput);
        mediaPlayer->play();
        emit audioDataProvided(audioOutput->device());
    }
    void receiveAudioData(QByteArray& audioData)  {
        QBuffer audioBuffer(&audioData);
        audioBuffer.open(QIODevice::ReadOnly);
        audioOutput->start(&audioBuffer);
    }

private:
    QScopedPointer<QAudioOutput> audioOutput; 
    QAudioFormat audioFormat;
    QAudioDecoder audioDecoder;
    QSharedPointer<QIODevice> sourceDevice;
};

class AudioCapture : public QObject
{
    Q_OBJECT

public:
    AudioCapture(QAudioFormat format)
    {
        audio = new QAudioInput(format, this);
    }
    AudioCapture()
    {
        QAudioFormat format;
        format.setSampleRate(8000);
        format.setChannelCount(1);
        format.setSampleSize(8);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);

        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
        if (!info.isFormatSupported(format)) {
            qWarning() << "Default format not supported, trying to use the nearest.";
            format = info.nearestFormat(format);
        }
        audio = new QAudioInput(format, this);
    }
    void start() {
        device = audio->start();
        connect(device, SIGNAL(readyRead()), SLOT(readData()));
    }
public slots:
    void readData() {
        QByteArray data = device->readAll();
        emit audioDataProvided(data);
    }
signals:
    void audioDataProvided(const QByteArray& audioData);
private:
    QAudioInput *audio;
    QIODevice *device;
};

class AudioStreamer : public QObject {
    Q_OBJECT

public:
    explicit AudioStreamer(QSharedPointer<QAbstractSocket> socket, QObject* parent = nullptr)
        : QObject(parent)
        , socket(socket)
        , multicastGroupAddress("239.255.0.1") // Example multicast group address
        , multicastPort(12345) // Example multicast port
    {
        this->socket->setSocketOption(QAbstractSocket::MulticastTtlOption, 1); // Set TTL to 1 for local network
        this->socket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, true); // Enable loopback for testing
        this->socket->bind(QHostAddress::AnyIPv4, multicastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        this->socket->joinMulticastGroup(multicastGroupAddress);
        connect(this, SIGNAL(audioDataProvided(QByteArray&)), this, SLOT(writeAudioDataBytes(QByteArray&)));
        connect(this, &AudioStreamer::audioDataProvided, this, &AudioStreamer::writeAudioDataBytes);
    }
    virtual ~AudioStreamer() {
        if (socket) {
            socket->disconnectFromHost();
        }
    }
    void receiveAudioData(QByteArray& audioData) override {
        QByteArray audioStream;
        QDataStream stream(&audioStream, QIODevice::WriteOnly);
        stream << audioData;
        if (socket->isOpen()) {
            socket->write(audioStream.data(), audioStream.size());
        }
    }
    QSharedPointer<QAbstractSocket> getSocket() const { return socket; }
public slots:
    void writeAudioDataBytes(QByteArray& audioStream)
    {
        QDataStream stream(&audioStream, QIODevice::WriteOnly);
        stream << audioData;
        if (socket->isOpen()) {
            socket->write(audioStream.data(), audioStream.size());
        }
    }
signals:
    void audioDataProvided(QByteArray& audioData);
private:
    QHostAddress multicastGroupAddress;
    quint16 multicastPort;
    QSharedPointer<QAbstractSocket> socket;
};

/**
 * @brief Class that controls the audio streaming and playing services.
 */
class AudioService : public QObject {
    Q_OBJECT
public:
    explicit AudioService(MessageType type, quint16 port, QObject* parent = nullptr)
        : QObject(parent), port(port)
    {
        switch (type)
        {
        case MessageType::StartAudioStreamRequest:     
            audioCapture = std::make_shared<AudioCapture>();
            audioCapture->start();
            streamerFactory = AudioStreamerFactory(QHostAddress::AnyIPv4, port, parent);
            audioStreamer = streamerFactory.create();
            // Stream the captured audio
            connect(audioCapture.get(), &AudioCapture::audioDataProvided, this, &AudioService::handleAudioDataProvided);
            break;
        case MessageType::StartAudioStreamResponse:
            // Start playing the received audio
            playerFactory = AudioPlayerFactory(QAudioFormat(), parent);
            audioPlayer = playerFactory.create();
            break;
        }
    }
    QSharedPointer<AudioPlayer> getAudioPlayer() const {
        return audioPlayer;
    }
    QSharedPointer<AudioStreamer> getAudioStreamer() const {
        return audioStreamer;
    }
    void setSSRCIdentifier(qint32 ssrcIdentifier) {
        this->ssrcIdentifier = ssrcIdentifier;
    }
public slots:
    void setAudioPlayer(QSharedPointer<QIODevice> sourceDevice, const QAudioFormat& audioFormat) {
        audioPlayer = QSharedPointer<AudioPlayer>(new AudioPlayer(sourceDevice, audioFormat));
        emit audioPlayerSet(audioPlayer->getAudioDecoder());
    }
    void setAudioStreamer(QSharedPointer<QAbstractSocket> targetDevice) {
        audioStreamer = QSharedPointer<AudioStreamer>(new AudioStreamer(targetDevice));
        emit audioStreamerSet(targetDevice);
    }
    void handleStartAudioStreaming(QSharedPointer<QIODevice> targetDevice) {
        // Instantiate and start AudioCapture in a new thread
        QThread *thread = new QThread;
        AudioCapture *audioCapture = new AudioCapture;
        audioCapture->moveToThread(thread);

        connect(thread, &QThread::started, audioCapture, &AudioCapture::start);
        connect(audioCapture, &AudioCapture::audioDataProvided, this, &AudioService::handleAudioDataProvided);
        thread->start();
        
        // Initialize audio streamer
        setAudioStreamer(targetDevice);
    }
    void handleStopAudioStreaming() {
        // Stop audio capture and audio streaming
        if (audioCapture) {
            audioCapture->stop();
            audioCapture->deleteLater();
        }
        audioStreamer.reset();
    }
    void handleAudioDataProvided(QByteArray audioData) {
        // Forward audio data to AudioStreamer
        audioStreamer->receiveAudioData(audioData);
    }
    void handleAudioMessage(QSharedPointer<AudioMessage> message)
    {
        switch (message->getType())
        {
        case MessageType::StartAudioStreamRequest:
            audioCapture = std::make_shared<AudioCapture>();
            audioCapture->start();
            connect(audioCapture.get(), &AudioCapture::audioDataProvided, this, [=](const QByteArray& data) {
                // Stream the captured audio
                audioStreamer->receiveAudioData(data);
            });
            break;
        case MessageType::StartAudioStreamResponse:
            audioPlayer = std::make_shared<AudioPlayer>(message->getDevice(), QAudioFormat());
            break;
        }
    }
signals:
    void messageReady(QSharedPointer<Message> message);
signals:
    void audioPlayerSet(QSharedPointer<QAudioDecoder> socket);
    void audioStreamerSet(QSharedPointer<QIODevice> socket);
    void audioDataRequested(QSharedPointer<QIODevice> socket);
private:
    QSharedPointer<AudioPlayer> audioPlayer;
    QSharedPointer<AudioStreamer> audioStreamer;
    QSharedPointer<AudioCapture> audioCapture;
};
//qint32 ssrcIdentifier;

/**
 * @brief MediaAudioPlayer is responsible for playing audio data from local audio source.
 */
class MediaAudioPlayer : public QObject {
    Q_OBJECT
public:
    explicit MediaAudioPlayer(QObject* parent = nullptr): QObject(parent)
    {
        mediaPlayer = new QMediaPlayer;
        audioOutput = new QAudioOutput;
        connect(mediaPlayer, SIGNAL(sourceChanged(QUrl)), this, SLOT(mediaPlayersourceChanged(QUrl)));
        connect(mediaPlayer, SIGNAL(audioOutputChanged()), this, SLOT(mediaPlayerAudioOutputChanged()));
    }
    void setSourceDevice(QSharedPointer<QIODevice> sourceDevice) {
        qDebug("MediaAudioPlayer::setSourceDevice");
        sourceDevice = sourceDevice;
        mediaPlayer->setSourceDevice(sourceDevice.data());
    }
    void setSource(QUrl newSource) {
        qDebug("MediaAudioPlayer::setSource");
        mediaPlayer->setSource(*source.data());
    }
    void setOutput(QAudioOutput* newOutput) {
        qDebug("MediaAudioPlayer::setOutput");
        mediaPlayer->setAudioOutput(newOutput);
    }
    void play() {
        qDebug("MediaAudioPlayer::play");
        audioOutput->setVolume(50);
        mediaPlayer->play();
    }

public slots:
    void mediaPlayersourceChanged(QUrl newSource)
    {
        qDebug("MediaAudioPlayer::mediaPlayersourceChanged");
        source = QSharedPointer<QUrl>(&newSource);
    }
    void mediaPlayerAudioOutputChanged()
    {
        qDebug("MediaAudioPlayer::mediaPlayerAudioOutputChanged");
        audioOutput = mediaPlayer->audioOutput();
    }
private:
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
    QSharedPointer<QUrl> source;
    QSharedPointer<QIODevice> sourceDevice;
};
#endif // AUDIOSERVICE_H

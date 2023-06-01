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
/*
        connect(sourceDevice.data(), &QIODevice::readyRead, this, &AudioPlayer::readAudioData);
        connect(this, &AudioPlayer::audioDataRequested, this, &AudioPlayer::provideAudioData);
        connect(this, &AudioPlayer::audioBufferRead, this, &AudioPlayer::provideAudioData);
        connect(sourceDevice.data(), &QIODevice::readyRead, this, &AudioPlayer::playAudioData);
*/
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
private:
    QAudioFormat audioFormat;
    QAudioDecoder audioDecoder;
    QSharedPointer<QIODevice> sourceDevice;
};

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
/*
        connect(mediaPlayer, &QMediaPlayer::sourceChanged, this, &MediaAudioPlayer::mediaPlayersourceChanged);
        connect(mediaPlayer, &QMediaPlayer::audioOutputChanged, this, &MediaAudioPlayer::mediaPlayerAudioOutputChanged);
*/
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


/**
 * @brief AudioStreamer is responsible for streaming audio data.
 */

class AudioStreamer : public QObject {
    Q_OBJECT

public:
    explicit AudioStreamer(QSharedPointer<QAbstractSocket> socket, QObject* parent = nullptr)
        : QObject(parent), socket(socket)
    {
        connect(this, SIGNAL(audioDataProvided(QByteArray&)), this, SLOT(writeAudioDataBytes(QByteArray&)));
/**/
        connect(this, &AudioStreamer::audioDataProvided, this, &AudioStreamer::writeAudioDataBytes);
    }
    virtual ~AudioStreamer() {
        if (socket) {
            socket->disconnectFromHost();
        }
    }
public slots:
    void writeAudioDataBytes(QByteArray& audioData)
    {
        QByteArray audioStream;
        QDataStream stream(&audioStream, QIODevice::WriteOnly);
        stream << audioData;
        if (socket->isOpen()) {
            socket->write(audioStream.data(), audioStream.size());
        }
    }
signals:
    void audioDataProvided(QByteArray& audioData);
private:
    QSharedPointer<QAbstractSocket> socket;
};

/**
 * @brief Class that controls the audio streaming and playing services.
 */
class AudioService : public QObject {
    Q_OBJECT
public:
    explicit AudioService(QSharedPointer<AudioPlayer> player, QSharedPointer<AudioStreamer> streamer, QObject* parent = nullptr)
        : QObject(parent), audioPlayer(std::move(player)), audioStreamer(std::move(streamer))
    {
        connect(audioStreamer.data(), &AudioStreamer::audioDataProvided, this, &AudioService::setAudioPlayer);
        connect(audioPlayer.data(), &AudioPlayer::audioDataRequested, this, &AudioService::setAudioStreamer);
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
signals:
    void audioDataReceived(QSharedPointer<QIODevice> socket);
    void audioBufferReceived(QSharedPointer<QByteArray> data);
    void audioPlayerSet(QSharedPointer<QAudioDecoder> socket);
    void audioStreamerSet(QSharedPointer<QIODevice> socket);
    void audioDataRequested(QSharedPointer<QIODevice> socket);
private:
    QSharedPointer<AudioPlayer> audioPlayer;
    QSharedPointer<AudioStreamer> audioStreamer;
    qint32 ssrcIdentifier;
};


#endif // AUDIOSERVICE_H

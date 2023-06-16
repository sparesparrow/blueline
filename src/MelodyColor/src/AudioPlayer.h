// AudioPlayer.h
#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QSharedPointer>
#include <QUrl>
#include <QMediaPlayer>
#include <QIODevice>
#include <QAudioSource>
#include <QMediaRecorder>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QAudioFrame>
#include <QAudioBuffer>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioDevice>
#include <QFile>
#include <QDataStream>
#include <QDebug>

/*! @brief Interface for playing audio data from any source. */
class IAudioPlayer : public QObject
{
    Q_OBJECT
public:
    IAudioPlayer(QObject* parent = nullptr): QObject(parent)
    {}
    virtual ~IAudioPlayer() = default;
    virtual void setSourceDevice(QSharedPointer<QIODevice> sourceDevice) = 0;
    virtual void setSource(QUrl newSource) = 0;
    virtual void setOutput(QAudioOutput* newOutput) = 0;
public slots:
    virtual void play() = 0;
signals:
    void audioDataRequested(QSharedPointer<QIODevice> targetDevice);
    void audioBufferRead(QSharedPointer<QAudioBuffer> targetDevice);
    void audioDataProvided(QAudioDevice targetDevice);
};

/*! @brief AudioPlayer is responsible for playing audio data from remote audio source. */
class AudioPlayer : public IAudioPlayer
{
    Q_OBJECT
public:
    explicit AudioPlayer(QSharedPointer<QIODevice> sourceDevice, const QAudioFormat& audioFormat, QObject* parent = nullptr)
        : IAudioPlayer(parent), sourceDevice(sourceDevice), audioFormat(audioFormat), audioDecoder(this),
        mediaPlayer(new QMediaPlayer), audioOutput(new QAudioOutput)
    {
        audioDecoder.setSourceDevice(sourceDevice.data());
        connect(&audioDecoder, &QAudioDecoder::bufferReady, this, &AudioPlayer::readAudioData);
        connect(sourceDevice.data(), &QIODevice::readyRead, this, &AudioPlayer::readAudioData);
        connect(this, &IAudioPlayer::audioBufferRead, this, &AudioPlayer::playAudioData);

        audioDecoder.start();
    }
    virtual ~AudioPlayer() = default;
    void setSourceDevice(QSharedPointer<QIODevice> sourceDevice) override {
        this->sourceDevice = sourceDevice;
        audioDecoder.setSourceDevice(sourceDevice.data());
    }
    void setSource(QUrl newSource) override {
        mediaPlayer->setSource(newSource);
    }
    void setOutput(QAudioOutput* newOutput) override {
        audioOutput.reset(newOutput);
        mediaPlayer->setAudioOutput(newOutput);
    }
    void play() override {
        mediaPlayer->play();
    }
public slots:
    /*! @brief Reads audio data from the decoder and emits audioBufferRead signal with the audio buffer. */
    void readAudioData() {
        QAudioBuffer audioBuffer = audioDecoder.read();
        QSharedPointer<QAudioBuffer> audioBufferPtr = QSharedPointer<QAudioBuffer>::create(audioBuffer);
        emit audioBufferRead(audioBufferPtr);
    }

    /*! @brief Starts the audio decoder and emits audioDataRequested signal with the target device. */
    void provideAudioData(QSharedPointer<QIODevice> targetDevice) {
        audioDecoder.start();
        emit audioDataRequested(targetDevice);
    }

    /*! @brief Plays the audio data and emits audioDataProvided signal with the audio device. */
    void playAudioData(QSharedPointer<QAudioBuffer> audioBufferPtr) {
        mediaPlayer->setSourceDevice(sourceDevice.data());
        mediaPlayer->setAudioOutput(audioOutput.data());
        mediaPlayer->play();
        emit audioDataProvided(audioOutput->device());
    }

private:
    QSharedPointer<QIODevice> sourceDevice;
    QAudioFormat audioFormat;
    QAudioDecoder audioDecoder;
    QMediaPlayer* mediaPlayer;
    QScopedPointer<QAudioOutput> audioOutput;
};

/*! @brief MediaAudioPlayer is responsible for playing audio data from local audio source. */
class MediaAudioPlayer : public IAudioPlayer
{
    Q_OBJECT
public:
    explicit MediaAudioPlayer(QObject* parent = nullptr): IAudioPlayer(parent)
    {
        mediaPlayer = new QMediaPlayer;
        audioOutput = new QAudioOutput;
        connect(mediaPlayer, SIGNAL(sourceChanged(QUrl)), this, SLOT(mediaPlayersourceChanged(QUrl)));
        connect(mediaPlayer, SIGNAL(audioOutputChanged()), this, SLOT(mediaPlayerAudioOutputChanged()));
    }
    void setSourceDevice(QSharedPointer<QIODevice> sourceDevice) override {
        this->sourceDevice = sourceDevice;
        mediaPlayer->setSourceDevice(sourceDevice.data());
    }
    void setSource(QUrl newSource) override {
        mediaPlayer->setSource(newSource);
    }
    void setOutput(QAudioOutput* newOutput) override {
        audioOutput = newOutput;
        mediaPlayer->setAudioOutput(newOutput);
    }
    void play() override {
        mediaPlayer->play();
    }

public slots:
    /*! @brief Sets the source of the media player. */
    void mediaPlayersourceChanged(QUrl newSource) {
        source = QSharedPointer<QUrl>::create(newSource);
    }

    /*! @brief Sets the audio output of the media player. */
    void mediaPlayerAudioOutputChanged() {
        audioOutput = mediaPlayer->audioOutput();
    }
private:
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
    QSharedPointer<QUrl> source;
    QSharedPointer<QIODevice> sourceDevice;
};

#endif // AUDIOPLAYER_H

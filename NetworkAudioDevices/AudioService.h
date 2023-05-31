// AudioService.h
#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <QObject>
#include <QByteArray>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAbstractSocket>
#include <QBuffer>
#include <QtMultimedia>
<<<<<<< HEAD
#include <QIODevice>
=======
#include <memory>

/**
 * @brief AudioPlayer is responsible for playing audio data.
 */
>>>>>>> 3ce8ad6 (AudioService redesign and factories)

class AudioPlayer : public QObject {
    Q_OBJECT
public:
<<<<<<< HEAD
    explicit AudioService(QObject* parent = nullptr) 
        : QObject(parent)
        , ssrcIdentifier(0)
    {}

    virtual ~AudioService() {}

    virtual void setSSRCIdentifier(qint32 ssrcIdentifier) {
        this->ssrcIdentifier = ssrcIdentifier;
    }

    virtual void receiveAudioData(const QByteArray &audioData) = 0;

protected:
    qint32 ssrcIdentifier;
};
// AudioPlayer.h
class AudioPlayer : public AudioService {
public:
    explicit AudioPlayer(QObject* parent = nullptr)
        : AudioService(parent)
        , audioOutput(new QAudioOutput(QAudioFormat()))
    {}

    // sets the SSRC identifier for synchronization, which can be used for implementing additional features related to synchronization or audio routing.
    void setSSRCIdentifier(qint32 ssrcIdentifier) override {
        this->ssrcIdentifier = ssrcIdentifier;
    }
    // receive and play audio data
    void receiveAudioData(const QByteArray& audioData) override {
=======
    explicit AudioPlayer(std::unique_ptr<QAudioSink> audioOutput, QObject* parent = nullptr)
        : QObject(parent), audioOutput(std::move(audioOutput)) {}
    void receiveAudioData(QByteArray& audioData)
    {
>>>>>>> 3ce8ad6 (AudioService redesign and factories)
        QBuffer audioBuffer(&audioData);
        audioBuffer.open(QIODevice::ReadOnly);
        audioOutput->start(&audioBuffer);
    }
private:
    std::unique_ptr<QAudioSink> audioOutput;
};

/**
 * @brief AudioStreamer is responsible for streaming audio data.
 */

class AudioStreamer : public QObject {
    Q_OBJECT

public:
    explicit AudioStreamer(QAbstractSocket* socket, QObject* parent = nullptr)
        : QObject(parent), socket(socket)
    {}
    virtual ~AudioStreamer() {
        if (socket) {
            socket->disconnectFromHost();
        }
    }
    void receiveAudioData(QByteArray& audioData)
    {
        QByteArray audioStream;
        QDataStream stream(&audioStream, QIODevice::WriteOnly);
        stream << audioData;
        if (socket->isOpen()) {
            socket->write(audioStream.data(), audioStream.size());
        }
    }
private:
    QSharedPointer<QAbstractSocket> socket;
};

/**
 * @brief Class that controls the audio streaming and playing services.
 */
class AudioService : public QObject {
    Q_OBJECT
public:
    explicit AudioService(std::unique_ptr<AudioPlayer> player, std::unique_ptr<AudioStreamer> streamer, QObject* parent = nullptr)
        : QObject(parent), audioPlayer(std::move(player)), audioStreamer(std::move(streamer)) {}
    void setAudioPlayer(std::unique_ptr<AudioPlayer> player) {
        audioPlayer = std::move(player);
    }
    void setAudioStreamer(std::unique_ptr<AudioStreamer> streamer) {
        audioStreamer = std::move(streamer);
    }
    AudioPlayer* getAudioPlayer() const {
        return audioPlayer.get();
    }
    AudioStreamer* getAudioStreamer() const {
        return audioStreamer.get();
    }
    void setSSRCIdentifier(qint32 ssrcIdentifier) {
        this->ssrcIdentifier = ssrcIdentifier;
    }
private:
    std::unique_ptr<AudioPlayer> audioPlayer;
    std::unique_ptr<AudioStreamer> audioStreamer;
    qint32 ssrcIdentifier;
};


#endif // AUDIOSERVICE_H

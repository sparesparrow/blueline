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
#include <QIODevice>

class AudioService : public QObject {
    Q_OBJECT

public:
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
        QBuffer audioBuffer(&audioData);
        audioBuffer.open(QIODevice::ReadOnly);
        audioOutput->start(&audioBuffer);
    }

private:
    QAudioOutput* audioOutput;
};


/*
    The AudioStreamer class include the multicast group address and port as member variables.
    The socket options for multicast TTL and loopback are set to control the behavior of multicast packets.
    The socket is bound to any available IPv4 address and the specified multicast port.
    The socket joins the multicast group using the joinMulticastGroup function.
*/
// AudioStreamer.h
class AudioStreamer : public AudioService {
public:
    explicit AudioStreamer(QObject* parent = nullptr)
        : AudioService(parent)
        , multicastGroupAddress("239.255.0.1") // Example multicast group address
        , multicastPort(12345) // Example multicast port
    {
        socket.setSocketOption(QAbstractSocket::MulticastTtlOption, 1); // Set TTL to 1 for local network
        socket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, true); // Enable loopback for testing
        socket.bind(QHostAddress::AnyIPv4, multicastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        socket.joinMulticastGroup(multicastGroupAddress);
    }
    /* Audio data is converted into a QByteArray stream using QDataStream.
       Audio stream is then sent to the multicast group using the writeDatagram function of the socket. */
    void receiveAudioData(const QByteArray& audioData) override {
        // Create a QByteArray to stream the audio data
        QByteArray audioStream;
        QDataStream stream(&audioStream, QIODevice::WriteOnly);
        stream << audioData;

        // Send the audioStream to the multicast group
        socket.writeDatagram(audioStream, multicastGroupAddress, multicastPort);
    }

private:
    QHostAddress multicastGroupAddress;
    quint16 multicastPort;
    QUdpSocket socket;
};


#endif // AUDIOSERVICE_H

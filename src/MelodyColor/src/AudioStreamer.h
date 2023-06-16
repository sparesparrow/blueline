// AudioStreamer.h
#ifndef AUDIOSTREAMER_H
#define AUDIOSTREAMER_H
#include <QObject>
#include <QUdpSocket>
#include <QSharedPointer>
#include <QVariant>
#include <QDataStream>
#include "AudioCapture.h"


/*! @brief Interface base class for streaming. */
class IStreamer : public QObject
{
    Q_OBJECT
public:
    IStreamer(QObject* parent = nullptr): QObject(parent)
    {}
    virtual ~IStreamer() = default;
    virtual QByteArray read() = 0;
    virtual void write(const QByteArray& data) = 0;
    virtual QIODevice* getTargetDevice() const = 0;
};
/*! @brief UdpStreamer is a class that streams data to a multicast group. */
class UdpStreamer : public IStreamer
{
 Q_OBJECT

public:
    explicit UdpStreamer(QSharedPointer<QUdpSocket> socket, QHostAddress multicastGroupAddress, quint16 multicastPort, QObject* parent = nullptr)
        : IStreamer(parent)
        , socket(socket)
    {
        setMulticastGroupAddress(multicastGroupAddress);
        setMulticastPort(multicastPort);
        connect(this, &UdpStreamer::audioDataProvided, this, &UdpStreamer::write);
    }
    virtual ~UdpStreamer() = default;

    void setMulticastGroupAddress(QHostAddress multicastGroupAddress) {
        this->multicastGroupAddress = multicastGroupAddress;
        socket->joinMulticastGroup(multicastGroupAddress);
    }

    void setMulticastPort(quint16 multicastPort) {
        this->multicastPort = multicastPort;
        socket->bind(multicastGroupAddress, multicastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
    }

    QHostAddress getMulticastGroupAddress() const {
        return multicastGroupAddress;
    }

    quint16 getMulticastPort() const {
        return multicastPort;
    }

    /*! @brief Receives audio data and streams it to the multicast group.
    * @param audioData The audio data to stream. */
    void receiveData(const QByteArray& audioData) {
        if (socket->isOpen()) {
            socket->writeDatagram(audioData, multicastGroupAddress, multicastPort);
        }
    }
    void write(const QByteArray& data) override {
        if (socket->isOpen()) {
            socket->writeDatagram(data, multicastGroupAddress, multicastPort);
        }
    }
    QByteArray read() override {
        QByteArray data;
        while (socket->hasPendingDatagrams()) {
            QByteArray tempData;
            tempData.resize(socket->pendingDatagramSize());
            socket->readDatagram(tempData.data(), tempData.size());
            data.append(tempData);
        }
        return data;
    }
    QIODevice* getTargetDevice() const override {
        return socket.data();
    }
signals:
    void audioDataProvided(const QByteArray& data);
private:
    QSharedPointer<QUdpSocket> socket;
    QHostAddress multicastGroupAddress;
    quint16 multicastPort;
};

class IUdpStreamerFactory {
public:
    virtual ~IUdpStreamerFactory() = default;
    virtual QSharedPointer<IStreamer> create(QSharedPointer<QUdpSocket> socket) = 0;
};

class UdpStreamerFactory : public IUdpStreamerFactory
{
public:
    explicit UdpStreamerFactory(QHostAddress multicastGroupAddress, quint16 multicastPort, QObject* parent = nullptr)
        : multicastGroupAddress(multicastGroupAddress), multicastPort(multicastPort), parent(parent)
    {}
    virtual ~UdpStreamerFactory() = default;

    QSharedPointer<IStreamer> create(QSharedPointer<QUdpSocket> socket) override
    {
        socket->setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
        socket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, true);
        socket->bind(QHostAddress::AnyIPv4, multicastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        socket->joinMulticastGroup(multicastGroupAddress);

        return QSharedPointer<IStreamer>(new UdpStreamer(socket, multicastGroupAddress, multicastPort, parent));
    }

private:
    QHostAddress multicastGroupAddress;
    quint16 multicastPort;
    QObject* parent;
};





template <typename T>
class ErrorHandlingDecorator : public T
{
public:
    ErrorHandlingDecorator(QSharedPointer<T> instance) : instance(std::move(instance)) {}
    QByteArray read() override {
        try {
            return instance->read();
        } catch(const std::exception& e) {
            // Handle error here
            qDebug("Error in return instance->read();");
            return QByteArray();
        }
    }
    void write(const QByteArray& data) override {
        try {
            instance->write(data);
        } catch(const std::exception& e) {
            // Handle error here
            qDebug("Error in instance->write(data);");
        }
    }
    void close() override {
        try {
            instance->close();
        } catch(const std::exception& e) {
            // Handle error here
            qDebug("Error in instance->close();");
        }
    }
private:
    QSharedPointer<T> instance;
};


/*
enum StreamerType {
    UdpSocket,
    Buffer,
    Bluetooth,
    LocalSocket,
    File
};

class StreamerFactory {
public:
    static QSharedPointer<IStreamer> createStreamer(StreamerType type, QHostAddress multicastGroupAddress = QHostAddress::Any, quint16 multicastPort = 0) {
        switch (type) {
            case UdpSocket:
                return QSharedPointer<IStreamer>(new UdpStreamerFactory(multicastGroupAddress, multicastPort));
            case Buffer:
                return QSharedPointer<BufferStreamer>::create();
            case Bluetooth:
                return QSharedPointer<BluetoothStreamer>::create();
            case LocalSocket:
                return QSharedPointer<LocalSocketStreamer>::create();
            case File:
                return QSharedPointer<FileStreamer>::create();
            default:
                return nullptr;
        }
    }
};

class BufferStreamer : public IStreamer {
public:
    // implementation for QBuffer handling
};

class BluetoothStreamer : public IStreamer {
public:
    // implementation for QBluetoothSocket handling
};

class LocalSocketStreamer : public IStreamer {
public:
    // implementation for QLocalSocket handling
};

class FileStreamer : public IStreamer {
public:
    // implementation for QFileDevice handling
};

*/
#endif // AUDIOSTREAMER_H

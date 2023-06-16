// AudioCapture.h
#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H
#include <QObject>
#include <QAudioInput>
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
#include <QUrl>

/**
* @brief The IAudioCapture class is an interface for capturing audio data.
*/
class IAudioCapture : public QObject
{
    Q_OBJECT
public:
    IAudioCapture(QObject* parent = nullptr): QObject(parent)
    {}
    virtual ~IAudioCapture() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
public slots:
    virtual void readData() = 0;
signals:
    void audioDataProvided(const QByteArray& audioData);
};

/**
* @brief The AudioCapture class captures audio data from the default audio input device.
*/
class AudioCapture : public IAudioCapture
{
    Q_OBJECT
public:
    /*! @brief AudioCapture constructor.
    * @param format The audio format to capture.
    * @param parent The parent object. */
    explicit AudioCapture(QObject* parent = nullptr): IAudioCapture(parent)
    {
        QAudioFormat format;
        format.setSampleRate(44100);
        format.setChannelCount(1);
        format.setSampleRate(16);
        format.setSampleFormat(QAudioFormat::Int16);
    }

    void start() override {
        connect(device, &QIODevice::readyRead, this, &AudioCapture::readData);
    }

    void stop() override {
        disconnect(device, &QIODevice::readyRead, this, &AudioCapture::readData);
    }
public slots:
    void readData() override {
        QByteArray data = device->readAll();
        emit audioDataProvided(data);
    }
private:
    QScopedPointer<QAudioInput> audioInput;
    QIODevice* device = nullptr;
};

/**
* @brief The AudioRecorder class records audio data to a file.
*/
class AudioRecorder : public AudioCapture
{
    Q_OBJECT
public:
    /*! @brief AudioRecorder constructor.
    * @param parent The parent object. */
    explicit AudioRecorder(QObject *parent = nullptr)
        : AudioCapture(parent), audioRecorder(new QMediaRecorder(this)), audioInput(nullptr), audioBuffer(nullptr)
    {
        connect(audioRecorder, &QMediaRecorder::recorderStateChanged, this, &AudioRecorder::handleStateChanged);
    }
    void start() override
    {
        audioRecorder->setOutputLocation(QUrl::fromLocalFile(filePath));
        QAudioFormat format;
        format.setSampleRate(44100);
        format.setChannelCount(1);
        format.setSampleRate(16);
        format.setSampleFormat(QAudioFormat::Int16);
        audioInput = new QAudioInput(this);
        connect(audioBuffer, &QIODevice::readyRead, this, &AudioRecorder::readData);
    }
    void stop() override
    {
        audioInput->deleteLater();
        audioBuffer = nullptr;
    }
    bool saveToWav(const QString& filePath, const QByteArray& audioData)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly))
        {
            qWarning() << "AudioRecorder::saveToWav - Failed to open file: " << filePath;
            return false;
        }
        QDataStream out(&file);
        out.setByteOrder(QDataStream::LittleEndian);
        // Write the "RIFF" chunk descriptor
        out.writeRawData("RIFF", 4);
        qint32 fileSize = audioData.size() + 36;
        out.writeRawData(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
        out.writeRawData("WAVE", 4);
        // Write the "fmt " sub-chunk descriptor
        out.writeRawData("fmt ", 4);
        qint32 fmtChunkSize = 16;
        out.writeRawData(reinterpret_cast<const char*>(&fmtChunkSize), sizeof(fmtChunkSize));
        qint16 audioFormat = 1; // PCM
        qint16 numChannels = 1; // Mono
        qint32 sampleRate = 44100; // CD quality
        qint32 byteRate = sampleRate * numChannels * 2; // 16-bit samples, 2 bytes per sample
        qint16 blockAlign = numChannels * 2; // 16-bit samples, 2 bytes per sample
        qint16 bitsPerSample = 16;
        out.writeRawData(reinterpret_cast<const char*>(&audioFormat), sizeof(audioFormat));
        out.writeRawData(reinterpret_cast<const char*>(&numChannels), sizeof(numChannels));
        out.writeRawData(reinterpret_cast<const char*>(&sampleRate), sizeof(sampleRate));
        out.writeRawData(reinterpret_cast<const char*>(&byteRate), sizeof(byteRate));
        out.writeRawData(reinterpret_cast<const char*>(&blockAlign), sizeof(blockAlign));
        out.writeRawData(reinterpret_cast<const char*>(&bitsPerSample), sizeof(bitsPerSample));
        // Write the "data" sub-chunk descriptor
        out.writeRawData("data", 4);
        qint32 dataChunkSize = audioData.size();
        out.writeRawData(reinterpret_cast<const char*>(&dataChunkSize), sizeof(dataChunkSize));
        out.writeRawData(audioData.constData(), audioData.size());
        return true;
    }
    /*! @brief getAudioBuffer gets the current audio buffer.
    * @return The current audio buffer. */
    QIODevice* getAudioBuffer() const {
        return audioBuffer;
    }
public slots:
    /*! @brief setAudioBuffer sets the audio buffer to read from.
    * @param device The audio buffer.
    * @return True if the audio buffer was set successfully, false otherwise. */
    bool setAudioBuffer(QIODevice *device) {
        if (audioBuffer == device) {
            return false;
        }
        if (audioBuffer != nullptr) {
            disconnect(audioBuffer, &QIODevice::readyRead, this, &IAudioCapture::readData);
        }
        audioBuffer = device;
        if (audioBuffer != nullptr) {
            connect(audioBuffer, &QIODevice::readyRead, this, &IAudioCapture::readData);
        }
        return true;
    }
private slots:
    void handleStateChanged(QMediaRecorder::RecorderState newState) {
        if (newState == QMediaRecorder::StoppedState)
        {
            const QString& filePath = audioRecorder->outputLocation().toLocalFile();
            const QByteArray& audioData = audioRecorder->audioSampleRate() == 16
                ? audioBuffer->readAll()
                : QByteArray();
            bool savingSuccessful = saveToWav(filePath, audioData);
            if (savingSuccessful) {
                qDebug() << "Audio saved successfully to: " << filePath;
            }
            else {
                qWarning() << "Failed to save audio to: " << filePath;
            }
        }
    }
private:
    QMediaRecorder* audioRecorder;
    QAudioInput* audioInput;
    QIODevice* audioBuffer;
    QString filePath;
};


#endif // AUDIOCAPTURE_H

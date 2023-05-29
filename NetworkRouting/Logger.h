
#include <optional>
#include <string>
#include <thread>
#include <chrono>
#include <QAudioDevice>
#include <QDebug>

class Logger
{
public:
    Logger(std::optional<std::string> msg = std::nullopt) 
    {
        if (msg)
            std::cerr << *msg << (errno ? strerror(errno) : "") <<  std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250'000));
    }
    virtual ~Logger() = default;
};

class DeviceInfoLogger
{
public:
    DeviceInfoLogger(std::shared_ptr<QAudioDevice> device)
    {
        qDebug() << "Device: " << device->description();
        qDebug() << "Device name: " << device->deviceName();
        qDebug() << "Device category: " << device->category();
        qDebug() << "Device availability: " << device->isAvailable();
        qDebug() << "Device format: " << device->supportedFormats();
        qDebug() << "Device sample rate: " << device->supportedSampleRates();
        qDebug() << "Device sample types: " << device->supportedSampleTypes();
        qDebug() << "Device byte orders: " << device->supportedByteOrders();
        qDebug() << "Device channel counts: " << device->supportedChannelCounts();
        qDebug() << "Device sample sizes: " << device->supportedSampleSizes();
        qDebug() << "Device volume: " << device->volume();
        qDebug() << "Device mute status: " << device->isMuted();
    }
    virtual ~DeviceInfoLogger() = default;
};

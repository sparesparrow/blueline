// main.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "AudioServiceFactory.h"
#include "AudioProcessor.h"
#include "AudioCapture.h"
#include "SpectrumVisualizer.h"
#include "AudioService.h"
#include "AudioStreamer.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // Create instances of the components
    AudioProcessor audioProcessor;
    VisualizerQml visualizerQml;

    // Create the controller and pass the instances
    AudioColorProvider controller(&visualizerQml);

    // Connect the observer to the controller
    QObject::connect(&controller, &AudioColorProvider::audioDataReady, &controller, &AudioColorProvider::readAudioData);
    QObject::connect(&audioProcessor, &AudioColorProvider::semitoneChanged, &visualizerQml, &VisualizerQml::updateVisualization);

    // Register AudioService and AudioColorProvider types
    qmlRegisterType<AudioService>("MelodyColor", 1, 0, "AudioService");
    qmlRegisterType<AudioColorProvider>("MelodyColor", 1, 0, "AudioColorProvider");

    // Load the QML file
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("audioVisualizer", &visualizerQml);
    engine.addImportPath(QStringLiteral("qrc:/"));
    engine.load(QUrl("qrc:/qml/main.qml"));
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

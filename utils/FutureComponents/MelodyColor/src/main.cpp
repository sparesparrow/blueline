#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QApplication>
#include "SpectrumAnalyser.h"
// main.cpp
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    SpectrumAnalyser spectrumAnalyser;
    engine.rootContext()->setContextProperty("spectrumAnalyser", &spectrumAnalyser);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "UserInterface.h"
#include "BluetoothManager.h"
#include "DeviceManager.h"
#include "AudioIOManager.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // Create instances of the necessary classes
    BluetoothManager bluetoothManager;
    DeviceManager deviceManager;
    AudioIOManager audioIOManager;
    UserInterface userInterface(&bluetoothManager, &deviceManager, &audioIOManager);

    // Create the QQmlApplicationEngine
    QQmlApplicationEngine engine;

    // Set the context property for UserInterface
    engine.rootContext()->setContextProperty("userInterface", &userInterface);

    // Load the main QML file
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

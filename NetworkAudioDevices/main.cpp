#include <QApplication>
#include "MainWindow.h"
#include "NetworkManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create the NetworkManager
    NetworkManager networkManager;

    // Create the main window
    MainWindow mainWindow(&networkManager);
    mainWindow.show();

    return app.exec();
}

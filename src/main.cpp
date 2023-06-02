// main.cpp
#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    NetworkManager networkManager;
    MainWindow mainWindow(&networkManager);
    mainWindow.show();

    return app.exec();
}

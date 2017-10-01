#include "mainwindow.h"
#include <QApplication>
#include <QCameraInfo>

//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QDebug>
#include <memory>
#include "communication/Crazyflie.h"

bool checkCameraAvailability()
{
    return (QCameraInfo::availableCameras().count() > 0);
}

int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        MainWindow w;
        w.show();

        return app.exec();
}



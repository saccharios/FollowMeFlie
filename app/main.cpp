#include "gui/main_window.h"
#include <QApplication>
#include <QCameraInfo>

//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QDebug>
#include <memory>
#include "crazyflie/crazy_flie.h"

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



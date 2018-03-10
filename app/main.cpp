#include "gui/main_window.h"
#include <QApplication>


#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QDebug>
#include <memory>
#include "crazyflie/crazy_flie.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return app.exec();
}



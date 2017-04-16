#include "mainwindow.h"
#include <QApplication>
#include <QCameraInfo>

//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QDebug>
#include <memory>
#include "communication/CCrazyflie.h"

bool checkCameraAvailability()
{
    return (QCameraInfo::availableCameras().count() > 0);
}

int main(int argc, char *argv[])
{
    //    QApplication app(argc, argv);
    //    MainWindow w;
    //    w.show();

    //    return app.exec();

    CCrazyRadio* crazyRadio = new CCrazyRadio("radio://0/80/250K");
    bool startRadio = crazyRadio->StartRadio();
    if(startRadio)
    {
        CCrazyflie *crazyFlie= new CCrazyflie(crazyRadio);
        crazyFlie->SetThrust(10001);

        // Enable sending the setpoints. This can be used to temporarily
        // stop updating the internal controller setpoints and instead
        // sending dummy packets (to keep the connection alive).
        crazyFlie->SetSendSetpoints(true);

        while(crazyFlie->Update())
        {
            // Main loop. Currently empty.
crazyFlie->SetThrust(60001);
            /* Examples to set thrust and RPY:

       // Range: 10001 - (approx.) 60000
       cflieCopter->setThrust(10001);

       // All in degrees. R/P shouldn't be over 45 degree (it goes
       // sidewards really fast!). R/P/Y are all from -180.0deg to 180.0deg.
       cflieCopter->setRoll(20);
       cflieCopter->setPitch(15);
       cflieCopter->setYaw(140); */

            // Important note: When quitting the program, please don't just
            // SIGINT (i.e. CTRL-C) it. The CCrazyflie class instance
            // cflieCopter must be deleted in order to call the destructor
            // which stops logging on the device. If you fail to do this
            // when quitting your program, your copter will experience some
            // kind of buffer overflow (because of a lot of logging messages
            // summing up without being collected) and you will have to
            // restart it manually. This is not being done in this
            // particular example. You have been warned.

            // Other than that, this example covers pretty much everything
            // basic you will need for controlling the copter.
        }

        delete crazyFlie;
    }
    else
    {
        std::cerr << "Could not connect to dongle. Did you plug it in?" << std::endl;
    }

    delete crazyRadio;
    return 0;


}



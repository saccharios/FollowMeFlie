#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>
#include <QCameraInfo>
#include <iostream>
//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QDebug>
#include "communication/Crazyflie.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _crazyRadio(),
    _crazyFlie(_crazyRadio),
    _crazyFlieCaller(_crazyFlie, parent)
{
    ui->setupUi(this);

    // Actions
    connect(&_crazyFlieCaller, SIGNAL(ConnectionTimeout()), this, SLOT(display_connection_timeout_box()));


    // Event loop on main window_
    QObject::connect(&_timer_t1, SIGNAL(timeout()), this, SLOT(display_act_values()));
    QObject::connect(&_timer_t1, SIGNAL(timeout()), this, SLOT(RePaintCameraViewPainter()));
//    _timer_t0.start(10); // time in ms
    _timer_t1.start(100); // time in ms
//    _timer_t2.start(500); // time in ms

    // Custom widgets
    ui->gridLayout->addWidget(&_cameraViewPainter);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::display_act_values()
 {
     ui->actRoll->setPlainText( QString::number(_crazyFlieCaller.GetRoll()));
     ui->actYaw->setPlainText( QString::number(_crazyFlieCaller.GetYaw()));
     ui->actPitch->setPlainText( QString::number(_crazyFlieCaller.GetPitch()));
     ui->actThrust->setPlainText( QString::number(_crazyFlieCaller.GetThrust()));
 }
void MainWindow::display_connection_timeout_box()
{
    QMessageBox msgBox;
    msgBox.setText("Could not connect to CrazyFlie.");
    msgBox.setInformativeText("Have you turned it on?");
    msgBox.exec();
}


void MainWindow::on_activateCamera_clicked()
{
    std::unique_ptr<QCamera> camera = nullptr;
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    qDebug()  << "available cameras: " << cameras.size();
    QString cameraName = "@device:pnp:\\\\?\\usb#vid_041e&pid_4095&mi_00#7&26faed27&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global";

    for( auto const & cameraInfo : cameras)
    {
        if (cameraInfo.deviceName() == cameraName)
        {
            camera = std::make_unique<QCamera>(cameraInfo);
        }
        qDebug() << cameraInfo.deviceName();
    }
    if(camera != nullptr)
    {
        qDebug() << "camera here!";
    }
}

void MainWindow::on_disconnectRadio_clicked()
{
//    _crazyRadio.StopRadio();
//    _crazyFlie.EnableStateMachine(false);

    QMessageBox msgBox;
    msgBox.setText("Feature not implemented yet.");
    msgBox.exec();

}

void MainWindow::on_connectRadio_clicked()
{
    _crazyRadio.StartRadio();
    if(_crazyRadio.RadioIsConnected())
    {
        _crazyFlie.StartConnecting(true);
    }
    else
    {
        _crazyFlie.StartConnecting(false);
        QMessageBox msgBox;
        msgBox.setText("Could not open Radio Dongle.");
        msgBox.setInformativeText("Have you plugged it in?");
        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch(ret)
        {
        default:
        case QMessageBox::Cancel:
            {
                break;
            }
        case QMessageBox::Retry:
        {
            on_connectRadio_clicked();
            break;
        }

        }

    }
}

void MainWindow::on_radioSettingsOptions_currentIndexChanged(int index)
{
    _crazyRadio.SetRadioSettings(index);
}

void MainWindow::on_exitApp_clicked()
{
    QCoreApplication::quit();
}


void MainWindow::on_pushButton_clicked()
{
    // With the newest firmware for the crazyflie 2.0, the motor need to be unlocked by sending a "thrust = 0" command
    // However, the following command does not do the trick.
    // I disabled the locking-functionality in the firmware.
//        _crazyFlie.SetThrust(0);
//        // Enable sending the setpoints. This can be used to temporarily
//        // stop updating the internal controller setpoints and instead
//        // sending dummy packets (to keep the connection alive).
//        _crazyFlie.SetSendSetpoints(true);
//        while(_crazyFlie.Update())
//        {
//            // Range: 10001 - (approx.) 60000

    if(_crazyFlie.IsConnected())
    {
            _crazyFlie.SetSendSetpoints(true);
            _crazyFlie.SetThrust(10001);
    }
            //            // Main loop. Currently empty.
//            //            Examples to set thrust and RPY:


//            // All in degrees. R/P shouldn't be over 45 degree (it goes
//            // sidewards really fast!). R/P/Y are all from -180.0deg to 180.0deg.
//            //       cflieCopter->setRoll(20);
//            //       cflieCopter->setPitch(15);
//            //       cflieCopter->setYaw(140);

//            // Important note: When quitting the program, please don't just
//            // SIGINT (i.e. CTRL-C) it. The Crazyflye class instance
//            // cflieCopter must be deleted in order to call the destructor
//            // which stops logging on the device. If you fail to do this
//            // when quitting your program, your copter will experience some
//            // kind of buffer overflow (because of a lot of logging messages
//            // summing up without being collected) and you will have to
//            // restart it manually. This is not being done in this
//            // particular example. You have been warned.

//            // Other than that, this example covers pretty much everything
//            // basic you will need for controlling the copter.
//        }
}



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
    _crazyFlieCaller(_crazyFlie, parent),
    _cameraViewPainter(_crazyFlieCaller.SensorValues().stabilizer.roll,
                       _crazyFlieCaller.SensorValues().stabilizer.yaw,
                       _crazyFlieCaller.SensorValues().stabilizer.pitch)
{
    ui->setupUi(this);

    // Actions
    connect(&_crazyFlieCaller, SIGNAL(ConnectionTimeout()), this, SLOT(display_connection_timeout_box()));

    // Event loop on main window_
    QObject::connect(&_timer_t1, SIGNAL(timeout()), this, SLOT(display_sensor_values()));
    QObject::connect(&_timer_t1, SIGNAL(timeout()), this, SLOT(RePaintCameraViewPainter()));
//    _timer_t0.start(10); // time in ms
    _timer_t1.start(100); // time in ms
//    _timer_t2.start(500); // time in ms

    // Custom widgets
    ui->Layout_CameraView->addWidget(&_cameraViewPainter);
    ui->Layout_TrackingColor->addWidget(&_trackingColor);





}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::display_sensor_values()
 {
     ui->actRoll->setPlainText( QString::number(_crazyFlieCaller.SensorValues().stabilizer.roll));
     ui->actYaw->setPlainText( QString::number(_crazyFlieCaller.SensorValues().stabilizer.yaw));
     ui->actPitch->setPlainText( QString::number(_crazyFlieCaller.SensorValues().stabilizer.pitch));
     ui->actThrust->setPlainText( QString::number(_crazyFlieCaller.SensorValues().stabilizer.thrust));
     ui->actAcc_x->setPlainText( QString::number(_crazyFlieCaller.SensorValues().acceleration.x));
     ui->actAcc_y->setPlainText( QString::number(_crazyFlieCaller.SensorValues().acceleration.y));
     ui->actAcc_z->setPlainText( QString::number(_crazyFlieCaller.SensorValues().acceleration.z));
     ui->actAcc_zw->setPlainText( QString::number(_crazyFlieCaller.SensorValues().acceleration.zw));
     ui->actBatterStatus->setPlainText( QString::number(_crazyFlieCaller.SensorValues().battery.state));
     ui->actBatteryLevel->setPlainText( QString::number(_crazyFlieCaller.SensorValues().battery.level));
     ui->actGyro_x->setPlainText( QString::number(_crazyFlieCaller.SensorValues().gyrometer.x));
     ui->actGyro_y->setPlainText( QString::number(_crazyFlieCaller.SensorValues().gyrometer.y));
     ui->actGyro_z->setPlainText( QString::number(_crazyFlieCaller.SensorValues().gyrometer.z));
     ui->actMag_x->setPlainText( QString::number(_crazyFlieCaller.SensorValues().magnetometer.x));
     ui->actMag_y->setPlainText( QString::number(_crazyFlieCaller.SensorValues().magnetometer.y));
     ui->actMag_z->setPlainText( QString::number(_crazyFlieCaller.SensorValues().magnetometer.z));
     ui->actAsl->setPlainText( QString::number(_crazyFlieCaller.SensorValues().barometer.asl));
     ui->actAslLong->setPlainText( QString::number(_crazyFlieCaller.SensorValues().barometer.aslLong));
     ui->actTemperature->setPlainText( QString::number(_crazyFlieCaller.SensorValues().barometer.temperature));
     ui->actPressure->setPlainText( QString::number(_crazyFlieCaller.SensorValues().barometer.pressure));

     // Also update connection status
     DisplayConnectionStatus();
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


void MainWindow::on_pushButton_setThrust_clicked()
{
    // With the newest firmware for the crazyflie 2.0, the motor need to be unlocked by sending a "thrust = 0" command
    // Update SF: I disabled the locking-functionality in the firmware.

    if(_crazyFlie.IsConnected())
    {
            _crazyFlie.SetSendSetpoints(true);
            _crazyFlie.SetThrust(20001);
    }
}



void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}
void MainWindow::DisplayConnectionStatus()
{
    QString connectionStatus;
    if( _crazyFlie.IsConnected())
    {
        connectionStatus = "Connected";
    }
    else if(_crazyFlie.IsConnecting())
    {
        connectionStatus = "Connecting";
    }
    else if(_crazyFlie.IsDisconnected())
    {
        connectionStatus = "Disconnected";
    }
    else
    {
        connectionStatus = "Connection Status unknown";
    }
    ui->display_ConnectionStatus->setPlainText(connectionStatus);
}


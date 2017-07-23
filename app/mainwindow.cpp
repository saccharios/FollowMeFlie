#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>

#include <iostream>
#include <QDebug>
#include "communication/Crazyflie.h"
#include <QMessageBox>
#include "opencv2/opencv.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _timer_t0(),
    _timer_t1(),
    _crazyRadio(),
    _crazyFlie(_crazyRadio),
    _crazyFlieCaller(_crazyFlie, parent),
    _cameraViewPainter(_crazyFlieCaller.SensorValues().stabilizer.roll,
                       _crazyFlieCaller.SensorValues().stabilizer.yaw,
                       _crazyFlieCaller.SensorValues().stabilizer.pitch),
     _trackingColor(),
    _camera(),
    _extractColor(_trackingColor.GetColor())
{
    ui->setupUi(this);

    // Actions
    connect(&_crazyFlieCaller, SIGNAL(ConnectionTimeout()), this, SLOT(display_connection_timeout_box()));

    // Event loop on main window
    QObject::connect(&_timer_t1, SIGNAL(timeout()), this, SLOT(display_sensor_values()));
    QObject::connect(&_timer_t1, SIGNAL(timeout()), this, SLOT(RePaintCameraViewPainter()));
    QObject::connect(&_timer_t0, SIGNAL(timeout()), this, SLOT(UpdateCamera()));
    _timer_t0.start(30); // time in ms
    _timer_t1.start(100); // time in ms
//    _timer_t2.start(500); // time in ms

    // Custom widgets
    ui->Layout_CameraView->addWidget(&_cameraViewPainter);
    ui->Layout_TrackingColor->addWidget(&_trackingColor);

    // Setup color bar sliders
    ui->verticalSlider_hue->setMinimum(0);
    ui->verticalSlider_hue->setMaximum(359);
    ui->verticalSlider_hue->setValue(_trackingColor.GetHue()); // Default value is the same as defined in trackingColor
    // TODO SF Can sat and value slider be removed??
    ui->verticalSlider_sat->setMinimum(0);
    ui->verticalSlider_sat->setMaximum(255);
    ui->verticalSlider_sat->setValue(255);
    ui->verticalSlider_value->setMinimum(0);
    ui->verticalSlider_value->setMaximum(255);
    ui->verticalSlider_value->setValue(255);

    // Connections
    QObject::connect(&_camera, SIGNAL(ImgReadyForDisplay(QImage const &)), &_cameraViewPainter, SLOT(SetImage(QImage const &)));
    QObject::connect(&_camera, SIGNAL(ImgReadyForProcessing(cv::Mat const &)), &_extractColor, SLOT(ProcessImage(cv::Mat const &)));


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
    ui->display_ConnectionStatus->setText(connectionStatus);
}


void MainWindow::on_verticalSlider_hue_valueChanged(int value)
{
    _trackingColor.SetHue(value);
    _trackingColor.repaint();
    ui->Hue_Num->setText(QString::number(value));
}

void MainWindow::on_verticalSlider_sat_valueChanged(int value)
{
    _trackingColor.SetSaturation(value);
    _trackingColor.repaint();
    ui->Sat_Num->setText(QString::number(value));
}

void MainWindow::on_verticalSlider_value_valueChanged(int value)
{
    _trackingColor.SetValue(value);
    _trackingColor.repaint();
    ui->Val_Num->setText(QString::number(value));
}

void MainWindow::UpdateCamera()
{
    _camera.Update();
}

void MainWindow::on_pushButton_CameraOnlyMode_clicked()
{
    bool activate = (_camera.GetState() == Camera::CameraState::DISABLED);
    if(activate)
    {
    _cameraViewPainter.SetCameraBackGround();
    }
    else
    {
        _cameraViewPainter.SetHorizon();
    }
    _camera.Activate(activate);
}

void MainWindow::on_pushButton_Stop_clicked()
{
    _camera.Activate(false);
}

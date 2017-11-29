#include "main_window.h"
#include "ui_mainwindow.h"
#include <memory>

#include <iostream>
#include <QDebug>
#include "communication/crazy_flie.h"
#include <QMessageBox>
#include "opencv2/opencv.hpp"
#include "actual_values_model.h"
#include "parameter_model.h"
#include <QTableView>

#include "qt_util.h"
#include "time_levels.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _radioDongle(),
    _crazyFlie(_radioDongle),
    ui(new Ui::MainWindow),
     _actualValuesTable(nullptr),
     _parameterTable(nullptr),
    _actualValuesModel(_crazyFlie.GetLogElements(), nullptr),
    _parameterModel(_crazyFlie.GetParameterElements(), nullptr),
    _timer_t0(),
    _timer_t1(),
    _timer_t2(),
    _timer_sr(),
    _cameraViewPainter(_crazyFlie.GetSensorValues().stabilizer.roll,
                       _crazyFlie.GetSensorValues().stabilizer.yaw,
                       _crazyFlie.GetSensorValues().stabilizer.pitch),
     _trackingColor(),
    _camera(),
    _extractColor(_trackingColor.GetColor()),
    _commander(_crazyFlie, crazyflieUpdateSamplingTime_seconds)
{
    ui->setupUi(this);
    // Event loop on main window
    // T0
    _timer_t0.start(crazyflieUpdateSamplingTime); // time in ms
    QObject::connect(&_timer_t0, SIGNAL(timeout()), this, SLOT(UpdateCrazyFlie()));
    // T1
    _timer_t1.start(cameraUpdateSamplingTime); // time in ms
    QObject::connect(&_timer_t1, SIGNAL(timeout()), this, SLOT(UpdateCamera()));

    // T2
    _timer_t2.start(guiUpdateSamplingTime); // time in ms
    QObject::connect(&_timer_t2, SIGNAL(timeout()), this, SLOT(UpdateConnectionStatus()));
    QObject::connect(&_timer_t2, SIGNAL(timeout()), &_actualValuesModel, SLOT(UpdateActualValues()));
    QObject::connect(&_timer_t2, SIGNAL(timeout()), this, SLOT(RePaintCameraViewPainter()));

    // Send and Receive packets
    _timer_sr.start(sendReceiveSamplingTime);
   QObject::connect(&_timer_sr, SIGNAL(timeout()), &_radioDongle, SLOT(SendPacketsNow()));
   QObject::connect(&_timer_sr, SIGNAL(timeout()), &_radioDongle, SLOT(ReceivePacket()));


    // Custom widgets
    ui->Layout_CameraView->addWidget(&_cameraViewPainter);
    ui->Layout_TrackingColor->addWidget(&_trackingColor);

    // Setup color bar sliders
    ui->verticalSlider_hue->setMinimum(0);
    ui->verticalSlider_hue->setMaximum(359);
    ui->verticalSlider_hue->setValue(_trackingColor.GetHue()); // Default value is the same as defined in trackingColor

    // Connections
    QObject::connect(&_camera, SIGNAL(ImgReadyForDisplay(QImage const &)), &_cameraViewPainter, SLOT(SetImage(QImage const &)));
    QObject::connect(&_camera, SIGNAL(ImgReadyForProcessing(cv::Mat const &)), &_extractColor, SLOT(ProcessImage(cv::Mat const &)));
    QObject::connect(&_crazyFlie, SIGNAL(ConnectionTimeout()), this, SLOT(DisplayConnectionTimeoutBox()));
    QObject::connect(&_crazyFlie, SIGNAL(NotConnecting()), this, SLOT(DisplayNotConnectingBox()));
    QObject::connect(&_crazyFlie.GetParameterTOC(), SIGNAL(ParameterRead(uint8_t const &)),
                     &_parameterModel, SLOT(UpdateParameter(uint8_t const &)));
    QObject::connect(&_parameterModel, SIGNAL( ParameterWrite(uint8_t, float)),
                     &_crazyFlie.GetParameterTOC(), SLOT( WriteParameter(uint8_t, float)));

    QObject::connect(&_radioDongle, SIGNAL(NewParameterPacket(CRTPPacket)) ,
                    &_crazyFlie.GetParameterTOC(), SLOT(ReceivePacket(CRTPPacket)));
    QObject::connect(&_radioDongle, SIGNAL(NewLoggerPacket(CRTPPacket)) ,
                     &_crazyFlie.GetLoggerTOC(), SLOT(ReceivePacket(CRTPPacket)));
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::UpdateConnectionStatus()
 {
     DisplayConnectionStatus();
 }
void MainWindow::DisplayConnectionTimeoutBox()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Connection Error");
    msgBox.setText("Connection Time out.");
    msgBox.exec();
}
void MainWindow::DisplayNotConnectingBox()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Connection Error");
    msgBox.setText("Could not connect to CrazyFlie.");
    msgBox.setInformativeText("Have you turned it on?");
    msgBox.exec();
}



void MainWindow::on_disconnectRadio_clicked()
{
//    _radioDongle.StopRadio();
//    _crazyFlie.EnableStateMachine(false);

    QMessageBox msgBox;
    msgBox.setText("Feature not implemented yet.");
    msgBox.exec();

}

void MainWindow::on_connectRadio_clicked()
{
    _radioDongle.StartRadio();
    if(_radioDongle.RadioIsConnected())
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

void MainWindow::on_radioSettings_Options_currentIndexChanged(int index)
{
    _radioDongle.SetRadioSettings(index);
}

void MainWindow::on_exitApp_clicked()
{
    QCoreApplication::quit();
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
    _commander.Stop();
}

void MainWindow::on_pushButton_hoverMode_clicked()
{
    _commander.ActivateHoverMode(true);
}


void MainWindow::UpdateCrazyFlie()
{
    _commander.Update();
    _crazyFlie.Update();
}

void MainWindow::on_pushButton_SafeLandingMode_clicked()
{
}


void MainWindow::on_pushButton_ActualValues_clicked()
{
    if(_actualValuesTable == nullptr)
    {
        _actualValuesTable = new QTableView();
        _actualValuesTable->setModel(&_actualValuesModel);
        qt_utils::SetupTableViewWidget(_actualValuesTable);
        _actualValuesTable->setWindowTitle("Actual Values");
        _actualValuesTable->show();
    }
    else
    {
        delete _actualValuesTable;
        _actualValuesTable = nullptr;
    }
}

void MainWindow::on_pushButton_ParameterTable_clicked()
{
    if(_parameterTable == nullptr)
    {
        _parameterTable = new QTableView();
        _parameterTable->setModel(&_parameterModel);

        qt_utils::SetupTableViewWidget(_parameterTable);
        _parameterTable->setWindowTitle("Parameter Table");
        _parameterTable->show();
    }
    else
    {
        delete _parameterTable;
        _parameterTable = nullptr;
    }
}
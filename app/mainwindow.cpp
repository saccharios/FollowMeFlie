#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>

#include <iostream>
#include <QDebug>
#include "communication/Crazyflie.h"
#include <QMessageBox>
#include "opencv2/opencv.hpp"
#include "actual_values_model.h"
#include "parameter_model.h"
#include <QTableView>


void SetupTableViewWidget(QTableView* tableView)
{
    // Hide vertical header
    tableView->verticalHeader()->hide();
    // Resize columns and rows to fit content
//    tableView->resizeColumnsToContents();
//    tableView->resizeRowsToContents();
    // Resize TableView Widget to match content size
    int w = 0;
    int h = 0;
    w += tableView->contentsMargins().left() + tableView->contentsMargins().right();
    h +=  tableView->contentsMargins().top()+ tableView->contentsMargins().bottom();
    h +=  tableView->horizontalHeader()->height();
    for (int i=0; i<tableView->model()->columnCount(); ++i)
    {
        w += tableView->columnWidth(i);
    }
    for (int i=0; i < 6; ++i) // Minimum 6 rows are shown.
    {
        h += tableView->rowHeight(i);
    }

    tableView->setMinimumWidth(w);
//    tableView->setMaximumWidth(w);
    tableView->setMinimumHeight(h);
//    tableView->setMaximumHeight(h);


    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
     _actualValuesTable(nullptr),
     _parameterTable(nullptr),
     _dataForActualValuesModel(),
    _actualValuesModel(_dataForActualValuesModel, nullptr),
    _parameterModel(_dataForActualValuesModel, nullptr),
    _timer_t0(),
    _timer_t1(),
    _crazyRadio(),
    _crazyFlie(_crazyRadio),
    _cameraViewPainter(_crazyFlie.GetSensorValues().stabilizer.roll,
                       _crazyFlie.GetSensorValues().stabilizer.yaw,
                       _crazyFlie.GetSensorValues().stabilizer.pitch),
     _trackingColor(),
    _camera(),
    _extractColor(_trackingColor.GetColor()),
    _commander(_crazyFlie)
{
    ui->setupUi(this);
    // Event loop on main window
    // T0
    _timer_t0.start(crazyflieUpdateSamplingTime); // time in ms
    QObject::connect(&_timer_t0, SIGNAL(timeout()), this, SLOT(UpdateCrazyFlie()));
    // T1
    _timer_t1.start(30); // time in ms
    QObject::connect(&_timer_t1, SIGNAL(timeout()), this, SLOT(UpdateCamera()));

    // T2
    _timer_t2.start(100); // time in ms
    QObject::connect(&_timer_t2, SIGNAL(timeout()), this, SLOT(display_sensor_values()));
    QObject::connect(&_timer_t2, SIGNAL(timeout()), this, SLOT(RePaintCameraViewPainter()));

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
    QObject::connect(&_crazyFlie, SIGNAL(ConnectionTimeout()), this, SLOT(display_connection_timeout_box()));
    QObject::connect(&_crazyFlie, SIGNAL(NotConnecting()), this, SLOT(display_not_connecting_box()));


    // For testing purposes
    TOCElement e1;
    e1.id = 0;
    e1.group = "Stabilizer";
    e1.name_only = "Acc_x";
    e1.name = e1.group + "." + e1.name_only;
    e1.type = ElementType::UINT8;
    e1.value = 10;
    _dataForActualValuesModel.emplace_back(e1);
    TOCElement e;
    e1.id = 4;
    e1.group = "Stabilizer";
    e1.name_only = "Acc_y";
    e1.name = e1.group + "." + e1.name_only;
    e1.type = ElementType::UINT8;
    e1.value = 20;
    _dataForActualValuesModel.emplace_back(e1);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::display_sensor_values()
 {
     ui->actRoll->setPlainText( QString::number(_crazyFlie.GetSensorValues().stabilizer.roll));
     ui->actYaw->setPlainText( QString::number(_crazyFlie.GetSensorValues().stabilizer.yaw));
     ui->actPitch->setPlainText( QString::number(_crazyFlie.GetSensorValues().stabilizer.pitch));
     ui->actThrust->setPlainText( QString::number(_crazyFlie.GetSensorValues().stabilizer.thrust));
     ui->actAcc_x->setPlainText( QString::number(_crazyFlie.GetSensorValues().acceleration.x));
     ui->actAcc_y->setPlainText( QString::number(_crazyFlie.GetSensorValues().acceleration.y));
     ui->actAcc_z->setPlainText( QString::number(_crazyFlie.GetSensorValues().acceleration.z));
     ui->actAcc_zw->setPlainText( QString::number(_crazyFlie.GetSensorValues().acceleration.zw));
     ui->actBatterStatus->setPlainText( QString::number(_crazyFlie.GetSensorValues().battery.state));
     ui->actBatteryLevel->setPlainText( QString::number(_crazyFlie.GetSensorValues().battery.level));
     ui->actGyro_x->setPlainText( QString::number(_crazyFlie.GetSensorValues().gyrometer.x));
     ui->actGyro_y->setPlainText( QString::number(_crazyFlie.GetSensorValues().gyrometer.y));
     ui->actGyro_z->setPlainText( QString::number(_crazyFlie.GetSensorValues().gyrometer.z));
     ui->actMag_x->setPlainText( QString::number(_crazyFlie.GetSensorValues().magnetometer.x));
     ui->actMag_y->setPlainText( QString::number(_crazyFlie.GetSensorValues().magnetometer.y));
     ui->actMag_z->setPlainText( QString::number(_crazyFlie.GetSensorValues().magnetometer.z));
     ui->actAsl->setPlainText( QString::number(_crazyFlie.GetSensorValues().barometer.asl));
     ui->actAslLong->setPlainText( QString::number(_crazyFlie.GetSensorValues().barometer.aslLong));
     ui->actTemperature->setPlainText( QString::number(_crazyFlie.GetSensorValues().barometer.temperature));
     ui->actPressure->setPlainText( QString::number(_crazyFlie.GetSensorValues().barometer.pressure));

     // Also update connection status
     DisplayConnectionStatus();
 }
void MainWindow::display_connection_timeout_box()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Connection Error");
    msgBox.setText("Connection Time out.");
    msgBox.exec();
}
void MainWindow::display_not_connecting_box()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Connection Error");
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

void MainWindow::on_radioSettings_Options_currentIndexChanged(int index)
{
    _crazyRadio.SetRadioSettings(index);
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
    _crazyRadio.ReadParameter();
}


void MainWindow::on_pushButton_ActualValues_clicked()
{
    if(_actualValuesTable == nullptr)
    {
        _actualValuesTable = new QTableView();
        _actualValuesTable->setModel(&_actualValuesModel);
        SetupTableViewWidget(_actualValuesTable);
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

        SetupTableViewWidget(_parameterTable);
        _parameterTable->setWindowTitle("Parameter Table");
        _parameterTable->show();
    }
    else
    {
        delete _parameterTable;
        _parameterTable = nullptr;
    }
}



void MainWindow::on_pushButton_TestAddElement_clicked()
{
    static int index = 0;
    ++index;
    TOCElement e;
    e.id = index;
    _dataForActualValuesModel.push_back(e);
}

void MainWindow::on_pushButton_TestRemoveElement_clicked()
{
    _dataForActualValuesModel.pop_back();
}

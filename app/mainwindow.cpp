#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>
#include <QCameraInfo>

//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_activateCamera_clicked(bool checked)
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

void MainWindow::on_disconnect_clicked(bool checked)
{

}

void MainWindow::on_connect_clicked(bool checked)
{

}

void MainWindow::on_radioSettingsOptions_currentIndexChanged(int index)
{

}

void MainWindow::on_exitApp_clicked()
{
    QCoreApplication::quit();
}


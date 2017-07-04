#include "camera.h"
#include <memory>
#include <QCamera>
#include <QDebug>
#include <QCameraInfo>
#include <iostream>
#include <QCameraViewfinder>
#include <QAbstractVideoSurface>

Camera::Camera () :
     _timerCamera(),
     _state(CameraState::DISABLED),
     _activated(false)
{
    QObject::connect(&_timerCamera, SIGNAL(timeout()), this, SLOT(Update()));
    _timerCamera.start(100); // 100 ms  loop
}

void Camera::Update()
{
    switch(_state)
    {
    default:
    case CameraState::DISABLED:
    {
        if(_activated)
        {
            _state = CameraState::CONNECTING;
        }

        break;
    }
    case CameraState::CONNECTING:
    {
        if(!_activated)
        {
            _state = CameraState::DISABLED;
            return;
        }
        if(_camera != nullptr)
        {
            QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
            //        qDebug()  << "available cameras: " << cameras.size();
            QString cameraName = "@device:pnp:\\\\?\\usb#vid_041e&pid_4095&mi_00#7&26faed27&1&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global";

            for( auto const & cameraInfo : cameras)
            {
                if (cameraInfo.deviceName() == cameraName)
                {
                    _camera = std::make_unique<QCamera>(cameraInfo);
                }
                // qDebug() << cameraInfo.deviceName();
            }
        }
        if(_camera != nullptr)
        {
//            std::cout << "Camera found\n";
            _state = CameraState::RUNNING;
        }
        else
        {
//            std::cout << "Camera not found\n";
            _state = CameraState::DISABLED;
            _activated = false;
        }


        break;
    }
    case CameraState::RUNNING:
    {
        if(!_activated)
        {
            // free camera
            _state = CameraState::DISABLED;
            _activated = false;
            return;
        }

        FetchImage();
        break;
    }
    }
}


void Camera::Activate(bool activate)
{
    _activated = activate;
}

void Camera::FetchImage()
{
    auto viewfinder = new QAbstractVideoSurface;
    _camera->setViewfinder(viewfinder);
    viewfinder->show();

   _camera->start(); // to start the viewfinder

}

void Camera::ProcessImage()
{

}

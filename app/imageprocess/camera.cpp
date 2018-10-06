#include "camera.h"
#include <memory>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <QImage>
#include <QCameraInfo>
#include "text_logger.h"
#include "math/types.h"

QImage Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copy
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy
     return dest;
}

Camera::Camera () :
     _state(CameraState::DISABLED),
     _activated(false),
     _capture(new cv::VideoCapture)
{
}

cv::KeyPoint Camera::_origin = cv::KeyPoint();

void Camera::Update()
{
    switch(_state)
    {
    default:
    case CameraState::DISABLED:
    {
        emit CameraIsRunning(false);
        if(_activated)
        {
            _state = CameraState::CONNECTING;
        }

        break;
    }
    case CameraState::CONNECTING:
    {
        emit CameraIsRunning(false);
        textLogger << "Found Cameras: " << QCameraInfo::availableCameras().count() << "\n";
        _capture->open(1); // 0 for laptop camera // 1 for crazyflie camera // 2 for creative camera
        if(_activated && _capture->isOpened())
        {
            cv::Size resolution;
            resolution.width = _capture->get(CV_CAP_PROP_FRAME_WIDTH);
            resolution.height = _capture->get(CV_CAP_PROP_FRAME_HEIGHT);

            SetResolution(resolution);
            _origin = ConvertMidPointToCameraCoord(MidPoint());

            _capture->set(CV_CAP_FFMPEG,true);
            _capture->set(CV_CAP_PROP_FPS,30);


//            textLogger << "Camera ok. Resolution is " << _resolution.width << " x " << _resolution.height << "\n";
            textLogger << "Settings: \n";
            textLogger << "CV_CAP_PROP_POS_MSEC  = " << _capture->get(CV_CAP_PROP_POS_MSEC ) << "\n";
            textLogger << "CV_CAP_PROP_POS_FRAMES   = " << _capture->get(CV_CAP_PROP_POS_FRAMES  )<< "\n";
            textLogger << "CV_CAP_PROP_POS_AVI_RATIO  = " << _capture->get(CV_CAP_PROP_POS_AVI_RATIO )<< "\n";
            textLogger << "CV_CAP_PROP_FPS  = " << _capture->get(CV_CAP_PROP_FPS)  << "\n";
            textLogger << "CV_CAP_PROP_FOURCC  = " << _capture->get(CV_CAP_PROP_FOURCC)  << "\n";
            textLogger << "CV_CAP_PROP_FRAME_COUNT  = " << _capture->get(CV_CAP_PROP_FRAME_COUNT)  << "\n";
            textLogger << "CV_CAP_PROP_FORMAT  = " << _capture->get(CV_CAP_PROP_FORMAT )<< "\n";
            textLogger << "CV_CAP_PROP_MODE  = " << _capture->get(CV_CAP_PROP_MODE)  << "\n";
            textLogger << "CV_CAP_PROP_BRIGHTNESS  = " << _capture->get(CV_CAP_PROP_BRIGHTNESS ) << "\n";
            textLogger << "CV_CAP_PROP_CONTRAST  = " << _capture->get(CV_CAP_PROP_CONTRAST ) << "\n";
            textLogger << "CV_CAP_PROP_SATURATION   = " << _capture->get(CV_CAP_PROP_SATURATION  ) << "\n";
            textLogger << "CV_CAP_PROP_HUE   = " << _capture->get(CV_CAP_PROP_HUE  )<< "\n";
            textLogger << "CV_CAP_PROP_GAIN   = " << _capture->get(CV_CAP_PROP_GAIN  )<< "\n";
            textLogger << "CV_CAP_PROP_EXPOSURE   = " << _capture->get(CV_CAP_PROP_EXPOSURE  )<< "\n";
            textLogger << "CV_CAP_PROP_CONVERT_RGB   = " << _capture->get(CV_CAP_PROP_CONVERT_RGB  )<< "\n";
            textLogger << "CV_CAP_PROP_CONTRAST  = " << _capture->get(CV_CAP_PROP_CONTRAST)  << "\n";
            InitializeTracking();
            _state = CameraState::RUNNING;
        }
        else if(!_activated)
        {
            _capture->release();
            _state = CameraState::DISABLED;
        }
        else if(_activated)
        {
            std::cout << "Failed to open camera\n";
            textLogger << "Failed to open camera\n";
        }
        break;
    }
    case CameraState::RUNNING:
    {
        emit CameraIsRunning(true);
        FetchAndImageReady();
        if ( !_activated)
        {
            _capture->release();
            _state = CameraState::DISABLED;
        }
        break;
    }
    }
}

void Camera::Activate(bool activate)
{
    _activated = activate;
}

void Camera::FetchAndImageReady()
{
    cv::Mat frame;
    FetchImage(frame);
    //    cv::imshow("Original Frame", (frame)); // Show camera stream in separate window
    auto image = Mat2QImage(frame);
    emit ImgReadyForDisplay(image);
    emit ImgReadyForProcessing(frame);
}

void Camera::FetchImage(cv::Mat & frame)
{
    (*_capture) >> frame;
    if( (frame).empty() )
    {
        textLogger << "ERROR!! Failed to capture frame\n";
        return;
    }
}

void Camera::InitializeTracking()
{
    cv::Mat frame;
    FetchImage(frame);
    FetchImage(frame); // Fetch twice, as the first image contains nothing
    emit ImgReadyForInitialization(frame);
}


bool Camera::IsDisabled() const
{
    return _state == CameraState::DISABLED;
}
bool Camera::IsActive() const
{
    return _state == CameraState::RUNNING;
}

#include "camera.h"
#include <memory>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <QImage>
#include <QCameraInfo>

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
cv::Size Camera::_resolution = cv::Size();
cv::Point2f Camera::_midPoint = cv::Point2f();

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
        std::cout << "Found Cameras: " << QCameraInfo::availableCameras().count() << std::endl;
        _capture->open(1); // 0 for laptop camera // 1 for crazyflie camera // 2 for creative camera
        if(_activated && _capture->isOpened())
        {
            _state = CameraState::RUNNING;
            _resolution.width = _capture->get(CV_CAP_PROP_FRAME_WIDTH);
            _resolution.height = _capture->get(CV_CAP_PROP_FRAME_HEIGHT);
            _midPoint = ConvertMidPointToCameraCoord({0.0, 0.0});

            _capture->set(CV_CAP_FFMPEG,true);
            _capture->set(CV_CAP_PROP_FPS,30);


            std::cout << "Camera ok. Resolution is " << _resolution.width << " x " << _resolution.height << "\n";
            std::cout << "Settings: \n";
            std::cout << "CV_CAP_PROP_POS_MSEC  = " << _capture->get(CV_CAP_PROP_POS_MSEC ) << std::endl;
            std::cout << "CV_CAP_PROP_POS_FRAMES   = " << _capture->get(CV_CAP_PROP_POS_FRAMES  )<< std::endl;
            std::cout << "CV_CAP_PROP_POS_AVI_RATIO  = " << _capture->get(CV_CAP_PROP_POS_AVI_RATIO )<< std::endl;
            std::cout << "CV_CAP_PROP_FPS  = " << _capture->get(CV_CAP_PROP_FPS)  << std::endl;
            std::cout << "CV_CAP_PROP_FOURCC  = " << _capture->get(CV_CAP_PROP_FOURCC)  << std::endl;
            std::cout << "CV_CAP_PROP_FRAME_COUNT  = " << _capture->get(CV_CAP_PROP_FRAME_COUNT)  << std::endl;
            std::cout << "CV_CAP_PROP_FORMAT  = " << _capture->get(CV_CAP_PROP_FORMAT )<< std::endl;
            std::cout << "CV_CAP_PROP_MODE  = " << _capture->get(CV_CAP_PROP_MODE)  << std::endl;
            std::cout << "CV_CAP_PROP_BRIGHTNESS  = " << _capture->get(CV_CAP_PROP_BRIGHTNESS ) << std::endl;
            std::cout << "CV_CAP_PROP_CONTRAST  = " << _capture->get(CV_CAP_PROP_CONTRAST ) << std::endl;
            std::cout << "CV_CAP_PROP_SATURATION   = " << _capture->get(CV_CAP_PROP_SATURATION  ) << std::endl;
            std::cout << "CV_CAP_PROP_HUE   = " << _capture->get(CV_CAP_PROP_HUE  )<< std::endl;
            std::cout << "CV_CAP_PROP_GAIN   = " << _capture->get(CV_CAP_PROP_GAIN  )<< std::endl;
            std::cout << "CV_CAP_PROP_EXPOSURE   = " << _capture->get(CV_CAP_PROP_EXPOSURE  )<< std::endl;
            std::cout << "CV_CAP_PROP_CONVERT_RGB   = " << _capture->get(CV_CAP_PROP_CONVERT_RGB  )<< std::endl;
            std::cout << "CV_CAP_PROP_CONTRAST  = " << _capture->get(CV_CAP_PROP_CONTRAST)  << std::endl;
            InitializeTracking();
        }
        else if(!_activated)
        {
            _capture->release();
            _state = CameraState::DISABLED;
        }
        else if(_activated)
        {
            std::cout << "failed to open camera\n";
        }
        break;
    }
    case CameraState::RUNNING:
    {
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
        std::cout << "ERROR!! Failed to capture frame\n";
        return;
    }
}

void Camera::InitializeTracking()
{
    cv::Mat frame;
    FetchImage(frame);
    emit ImgReadyForInitialization(frame);
}

std::vector<cv::KeyPoint> Camera::ConvertCameraToMidPointCoord(std::vector<cv::KeyPoint> const & keyPoints)
{
    std::vector<cv::KeyPoint> keyPointsMidPtCoord;
    for(auto const & point : keyPoints)
    {
        cv::KeyPoint keyPt = point;
        keyPt.pt = ConvertCameraToMidPointCoord(keyPt.pt);
        keyPointsMidPtCoord.push_back(keyPt);
    }
    return keyPointsMidPtCoord;
}

cv::Point2f  Camera::ConvertCameraToMidPointCoord(cv::Point2f cameraPt)
{
    cv::Point2f midPointCoord;
    midPointCoord.x = cameraPt.x - _resolution.width / 2;
    midPointCoord.y = -cameraPt.y + _resolution.height/ 2;
    return midPointCoord;
}

cv::Point2f  Camera::ConvertMidPointToCameraCoord(cv::Point2f midPt )
{
    cv::Point2f cameraCoord;
    cameraCoord.x = midPt.x + _resolution.width/2;
    cameraCoord.y = -midPt.y + _resolution.height/2;
    return cameraCoord;
}

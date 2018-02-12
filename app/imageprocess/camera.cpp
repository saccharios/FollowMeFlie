#include "camera.h"
#include <memory>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <QImage>

QImage Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}


Camera::Camera () :
     _state(CameraState::DISABLED),
     _activated(false),
     _capture(new cv::VideoCapture)
{
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

        _capture->open(0);
        if(_activated && _capture->isOpened())
        {
            _state = CameraState::RUNNING;
            _resolution.width = _capture->get(CV_CAP_PROP_FRAME_WIDTH);
            _resolution.height = _capture->get(CV_CAP_PROP_FRAME_HEIGHT);


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
        FetchImage();
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




void Camera::FetchImage()
{

    cv::Mat frame;

    (*_capture) >> frame;
    if( (frame).empty() )
    {
        std::cout << "ERROR!! Failed to capture frame\n";
        return;
    }
//    cv::imshow("Original Frame", (frame)); // Show camera stream in separate window
    auto image = Mat2QImage(frame);
    emit ImgReadyForDisplay(image);
    emit ImgReadyForProcessing(frame);
}


cv::Point2f  Camera::ConvertCameraToMidPointCoord(cv::Point2f cameraPt, cv::Size size)
{
    cv::Point2f midPointCoord;
    midPointCoord.x = cameraPt.x - size.width / 2;
    midPointCoord.y = -cameraPt.y + size.height/ 2;
    return midPointCoord;
}

cv::Point2f  Camera::ConvertMidPointToCameraCoord(cv::Point2f midPt, cv::Size size)
{
    cv::Point2f cameraCoord;
    cameraCoord.x = midPt.x + size.width/2;
    cameraCoord.y = -midPt.y + size.height/2;
    return cameraCoord;
}

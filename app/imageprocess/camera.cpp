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

bool EnumerateCameras(std::vector<int> & camIdx)
{
//    std::cout << "Searching for cameras IDs...";
//    camIdx.clear();
//    for(int idx=28; idx<30; idx++)
//    {
//        cv::VideoCapture cap(idx);       // open the camera
//        if(cap.isOpened())           // check if we succeeded
//        {
//            camIdx.push_back(idx);  // ad the ID to list of available cameras
//            std::cout << idx << " OK\n";
//        }
//        cap.release();
//    }
//    std::cout << std::endl << camIdx.size() << " cam(s) available";

//    return (camIdx.size()>0); // returns success
}
Camera::Camera () :
     _state(CameraState::DISABLED),
     _activated(false),
     _capture(new cv::VideoCapture)
{
    std::vector<int> camIdx;
    EnumerateCameras(camIdx);
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
        }
        else if(!_activated)
        {
            _capture->release();
            _state = CameraState::DISABLED;
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
    if( (frame).empty() ) return; // end of video stream
//    cv::imshow("Original Frame", (frame)); // Show camera stream in separate window

    auto image = Mat2QImage(frame);
    emit ImgReadyForDisplay(image);
    emit ImgReadyForProcessing(frame);
}


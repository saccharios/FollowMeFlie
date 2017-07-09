#include "camera.h"
#include <memory>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"

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
    if( frame.empty() ) return; // end of video stream
    cv::imshow("this is you, smile! :)", frame);
}

void Camera::ProcessImage()
{

}

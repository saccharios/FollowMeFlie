#include "camera.h"
#include <memory>
#include <iostream>

//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/opencv.hpp"

Camera::Camera () :
     _state(CameraState::DISABLED),
     _activated(false)
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



        break;
    }
    case CameraState::RUNNING:
    {

        FetchImage();
        break;
    }
    }
}



void Camera::Activate(bool activate)
{
    _activated = activate;

//    cv::VideoCapture cap;
//    // open the default camera, use something different from 0 otherwise;
//    // Check VideoCapture documentation.
////    if(!cap.open(1))
////    {
////        return 0;
////    }
//    cap.open(0);
//    while(true)
//    {
//          cv::Mat frame;
//          cap >> frame;
//          if( frame.empty() ) break; // end of video stream
//          cv::imshow("this is you, smile! :)", frame);
//          if( cv::waitKey(10) == 27 ) break; // stop capturing by pressing ESC
//    }



}

void Camera::FetchImage()
{



}

void Camera::ProcessImage()
{

}

#pragma once

#include<memory>

namespace cv {
class VideoCapture;
}

class Camera
{
public:
    enum class CameraState
    {
        DISABLED = 0,
        CONNECTING = 1,
        RUNNING = 2,
    };

    Camera();
    void Activate(bool activate);
    void Update();
    CameraState GetState() const {return _state; }

private:
    CameraState _state;
    bool _activated;

    cv::VideoCapture* _capture;

    void FetchImage();
    void ProcessImage();
};

#pragma once
#include <QObject>
#include<memory>
#include "opencv2/opencv.hpp"


namespace cv {
class VideoCapture;
}

class Camera  : public QObject
{
    Q_OBJECT
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
signals:
    void ImgReadyForDisplay(QImage const &);
    void ImgReadyForProcessing(cv::Mat const &);
private:
    CameraState _state;
    bool _activated;

    cv::VideoCapture* _capture;

    void FetchImage();
};

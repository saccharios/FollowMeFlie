#pragma once
#include <QObject>
#include<memory>

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
    void ImageReady(QImage const &);
private:
    CameraState _state;
    bool _activated;

    cv::VideoCapture* _capture;

    void FetchImage();
    void ProcessImage();
};

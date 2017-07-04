#pragma once
#include <QObject>
#include <QTimer>
#include<memory>
#include <QCamera>

class Camera : public QObject
{
    Q_OBJECT

    using uptrQCam = std::unique_ptr<QCamera> ;
    enum class CameraState
    {
        DISABLED = 0,
        CONNECTING = 1,
        RUNNING = 2,
    };

public:
    Camera();
    void Activate(bool activate);
private slots:
    void Update();
private:
    QTimer _timerCamera;
    CameraState _state;
    bool _activated;
    uptrQCam _camera;

    void FetchImage();
    void ProcessImage();
};

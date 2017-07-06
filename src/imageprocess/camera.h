#pragma once

#include<memory>

class Camera
{
    enum class CameraState
    {
        DISABLED = 0,
        CONNECTING = 1,
        RUNNING = 2,
    };

public:
    Camera();
    void Activate(bool activate);
    void Update();

private:
    CameraState _state;
    bool _activated;

    void FetchImage();
    void ProcessImage();
};

#pragma once
#include <QObject>
#include <QTimer>
#include<memory>
#include "imageprocess/camera.h"

class CameraCaller : public QObject
{
    Q_OBJECT

public:
    CameraCaller();
    void Activate(bool activate);
private slots:
    void Update();
private:
    QTimer _timerCamera;
    Camera _camera;
};

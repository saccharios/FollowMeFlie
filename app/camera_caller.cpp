#include "camera_caller.h"

CameraCaller::CameraCaller () :
     _timerCamera(),
     _camera()
{
    QObject::connect(&_timerCamera, SIGNAL(timeout()), this, SLOT(Update()));
    _timerCamera.start(100); // 100 ms  loop
}

void CameraCaller::Update()
{
    _camera.Update();
}


void CameraCaller::Activate(bool activate)
{
    _camera.Activate(activate);
}

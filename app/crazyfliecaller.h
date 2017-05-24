#ifndef CRAZYFLIECALLER_H
#define CRAZYFLIECALLER_H

#include <QObject>
#include <QTimer>
#include <communication/Crazyflie.h>
class CrazyFlieCaller : public QObject
{
    Q_OBJECT
public:
    explicit CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent = 0);

public slots:
    void Update();
    void CheckActValueChanged();
    float GetRoll() const;
    float GetYaw() const;
    float GetPitch() const;
    int GetThrust() const;

signals:
    void ActValueChanged();

private:
        QTimer _timer;
        Crazyflie & _crazyFlie;
        float _roll = 0;
        float _yaw = 0;
        float _pitch = 0;
        int _thrust = 0;
};

#endif // CRAZYFLIECALLER_H

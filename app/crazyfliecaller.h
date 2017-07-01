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

    float const & GetRollRef() const;
    float const & GetYawRef() const;
    float const & GetPitchRef() const;
public slots:
    void Update();
//    float GetRoll() const;
    float const & GetRoll() const;
    float GetYaw() const;
    float GetPitch() const;
    int GetThrust() const;
    float GetAltitude() const;
    void CheckConnectionTimeout();

signals:
    void ConnectionTimeout();

private:
        QTimer _timer_t0;
        QTimer _timer_t1;
        QTimer _timer_t2;
        Crazyflie & _crazyFlie;

};

#endif // CRAZYFLIECALLER_H

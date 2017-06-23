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
    void UpdateActValueTime();
    float GetRoll() const;
    float GetYaw() const;
    float GetPitch() const;
    int GetThrust() const;
    void CheckConnectionTimeout();

signals:
    void UpdateActValues();
    void ConnectionTimeout();

private:
        QTimer _timer_t0;
        QTimer _timer_t1;
        QTimer _timer_t2;
        Crazyflie & _crazyFlie;

};

#endif // CRAZYFLIECALLER_H

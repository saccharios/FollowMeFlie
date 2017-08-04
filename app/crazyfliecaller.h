#ifndef CRAZYFLIECALLER_H
#define CRAZYFLIECALLER_H

#include <QObject>
#include <QTimer>
#include <communication/Crazyflie.h>
// CrazyFlieCaller class is used as a wrapper class for the CrazyFlie for signals and slots.
class CrazyFlieCaller : public QObject
{
    Q_OBJECT
public:
    explicit CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent = 0);

public slots:
    void Update();
    void CheckConnectionTimeout();

signals:
    void ConnectionTimeout();

private:
        QTimer _timer_t0;
        QTimer _timer_t2;
        Crazyflie & _crazyFlie;
};

#endif // CRAZYFLIECALLER_H

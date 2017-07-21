#ifndef CRAZYFLIECALLER_H
#define CRAZYFLIECALLER_H

#include <QObject>
#include <QTimer>
#include <communication/Crazyflie.h>
// TODO SF Is this class really needed??
class CrazyFlieCaller : public QObject
{
    Q_OBJECT
public:
    explicit CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent = 0);

    SensorValues const & SensorValues() const { return _crazyFlie.GetSensorValues();}
public slots:
    void Update();
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

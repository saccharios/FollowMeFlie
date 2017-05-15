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

signals:

public slots:
    void Update();
private:
        QTimer _timer;
        Crazyflie & _crazyFlie;
};

#endif // CRAZYFLIECALLER_H

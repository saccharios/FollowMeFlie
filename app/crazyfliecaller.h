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
    void CheckRollChanged();
    float GetRoll() const;

signals:
    void RollChanged();

private:
        QTimer _timer;
        Crazyflie & _crazyFlie;
        double _roll = 0;
};

#endif // CRAZYFLIECALLER_H

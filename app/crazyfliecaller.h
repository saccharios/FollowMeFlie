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

signals:
    void UpdateActValues();

private:
        QTimer _timerUpdate;
        QTimer _timerDisplay;
        Crazyflie & _crazyFlie;

};

#endif // CRAZYFLIECALLER_H

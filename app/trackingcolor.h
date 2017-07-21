#pragma once
#include <QWidget>

class TrackingColor : public QWidget
{
    Q_OBJECT
public:
    TrackingColor() : _trackingColor(100,1,1) {}
     void paintEvent(QPaintEvent* /*event*/) override;
     QColor const & GetTrackingColor(){return _trackingColor;}
     void SetHue(int hue);
     void SetSaturation(int sat);
     void SetValue(int value);

signals:
     void ColorChanged(QColor const &);
private:
     QColor  _trackingColor;
};

#pragma once
#include <QWidget>

class TrackingColor : public QWidget
{
    Q_OBJECT
public:
    TrackingColor() : _trackingColor(QColor::fromHsv(296,255,255)) {} // pink
     void paintEvent(QPaintEvent* /*event*/) override;
     QColor const & GetColor(){return _trackingColor;}
     void SetHue(int hue);
     int GetHue();
     void SetSaturation(int sat);
     void SetValue(int value);

signals:
     void ColorChanged(QColor const &);
private:
     QColor  _trackingColor;
};

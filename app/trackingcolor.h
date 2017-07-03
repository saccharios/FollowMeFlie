#pragma once
#include <QWidget>

class TrackingColor : public QWidget
{
public:
    TrackingColor() : _trackingColor(100,1,1) {}
     void paintEvent(QPaintEvent* /*event*/) override;
     QColor const & GetTrackingColor(){return _trackingColor;}
public slots:
      void SetColor(int h, int s, int v) {_trackingColor.setHsv( h,  s,  v);}
private:
     QColor  _trackingColor;
};

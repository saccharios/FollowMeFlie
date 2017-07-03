#include "trackingcolor.h"

void TrackingColor::paintEvent(QPaintEvent* /*event*/)
{
    QPalette pal;
    pal.setColor(QPalette::Background, _trackingColor);
    setAutoFillBackground(true);
    setPalette(pal);
    show();
}

void TrackingColor::SetHue(int hue)
{
    _trackingColor.setHsv(hue, _trackingColor.saturation(), _trackingColor.value());
}

void TrackingColor::SetSaturation(int sat)
{
    _trackingColor.setHsv(_trackingColor.hue(),sat, _trackingColor.value());
}

void TrackingColor::SetValue(int value)
{
    _trackingColor.setHsv( _trackingColor.hue(), _trackingColor.saturation(), value);
}



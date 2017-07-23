#include "trackingcolor.h"

void TrackingColor::paintEvent(QPaintEvent* /*event*/)
{
    // Display color in the windows below the three bars
    QPalette pal;
    pal.setColor(QPalette::Background, _trackingColor);
    setAutoFillBackground(true);
    setPalette(pal);
    show();
}

void TrackingColor::SetHue(int hue)
{
    _trackingColor.setHsv(hue, _trackingColor.saturation(), _trackingColor.value());
    emit ColorChanged(_trackingColor);
}

void TrackingColor::SetSaturation(int sat)
{
    _trackingColor.setHsv(_trackingColor.hue(),sat, _trackingColor.value());
    emit ColorChanged(_trackingColor);
}

void TrackingColor::SetValue(int value)
{
    _trackingColor.setHsv( _trackingColor.hue(), _trackingColor.saturation(), value);
    emit ColorChanged(_trackingColor);
}

int TrackingColor::GetHue()
{
    int hue = 0;
    int sat = 0;
    int val = 0;
   _trackingColor.getHsv(&hue, &sat, &val);
   return hue;
}

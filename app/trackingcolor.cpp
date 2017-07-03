#include "trackingcolor.h"

void TrackingColor::paintEvent(QPaintEvent* /*event*/)
{
    QPalette pal;
    pal.setColor(QPalette::Background, _trackingColor);
    setAutoFillBackground(true);
    setPalette(pal);
    show();
}

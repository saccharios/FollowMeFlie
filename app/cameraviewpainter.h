#pragma once
#include <QWidget>
#include <QImage>
QPoint World2CameraCoord(QPointF point, QWidget* widget);
class BackGroundBase
{
public:
    virtual void DrawGround(QPainter & painter, float roll, float pitch, float x_max, float y_max) = 0;
};

class Horizon : public BackGroundBase
{
public:
    Horizon(QWidget * widget): _widget(widget) {}
    void DrawGround(QPainter & painter, float roll, float pitch, float x_max, float y_max) override;
private:
    QWidget * _widget;
};

class CameraBackGround : public BackGroundBase
{
public:
    CameraBackGround(QWidget * widget): _widget(widget) {}
    void DrawGround(QPainter & painter, float roll, float pitch, float x_max, float y_max) override;
    void SetImage(QImage const & img) {_image = img;}
private:
    QWidget * _widget;
    QImage  _image;
};

class CameraViewPainter : public QWidget
{
    Q_OBJECT
public:
    CameraViewPainter(float const & roll, float const & yaw, float const & pitch);
    void paintEvent(QPaintEvent* /*event*/) override;
public slots:
    void SetHorizon() {_backGround = &_horizon;}
    void SetCameraBackGround() {_backGround = &_cameraBackGround;}
    void SetImage(QImage const & img) {_cameraBackGround.SetImage(img);}
private:
    static constexpr qreal _x_max = 60; // 2*yaw_max is the total x coord
    static constexpr qreal _y_max = 30; // 2*pitch_max is the total y coord

    float const & _roll;
    float const & _yaw;
    float const & _pitch;


    void PaintHorizontalLine(QPainter & painter, float roll,float pitch);
    void PaintVerticalLine(QPainter & painter, float roll, float yaw);
    BackGroundBase * _backGround;
    Horizon _horizon;
    CameraBackGround _cameraBackGround;

};





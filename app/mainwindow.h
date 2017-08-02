#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "communication/CrazyRadio.h"
#include "communication/CrazyFlie.h"
#include "crazyfliecaller.h"
#include "cameraviewpainter.h"
#include "trackingcolor.h"
#include "imageprocess/camera.h"
#include "imageprocess/extractcolor.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_disconnectRadio_clicked();
    void on_connectRadio_clicked();
    void on_radioSettingsOptions_currentIndexChanged(int index);
    void on_exitApp_clicked();
    void display_sensor_values();
    void display_connection_timeout_box();

    void RePaintCameraViewPainter() {_cameraViewPainter.repaint();}
    void on_actionExit_triggered();

    void on_pushButton_setThrust_clicked();

    void on_verticalSlider_hue_valueChanged(int value);

    void on_pushButton_CameraOnlyMode_clicked();

    void on_pushButton_Stop_clicked();

    void UpdateCamera();

private:
    // GUI related
    Ui::MainWindow *ui;
    QTimer _timer_t0;
    QTimer _timer_t1;
//    QTimer _timer_t2;

    // Function related
    CrazyRadio _crazyRadio;
    Crazyflie _crazyFlie;
    CrazyFlieCaller _crazyFlieCaller;
    CameraViewPainter _cameraViewPainter;
    TrackingColor _trackingColor;
    Camera _camera;
    ExtractColor _extractColor;

    void DisplayConnectionStatus();
};

#endif // MAINWINDOW_H

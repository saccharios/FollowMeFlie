#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "communication/CrazyRadio.h"
#include "communication/CrazyFlie.h"
#include "crazyfliecaller.h"
#include "cameraviewpainter.h"

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
    void on_activateCamera_clicked();
    void on_disconnectRadio_clicked();
    void on_connectRadio_clicked();
    void on_radioSettingsOptions_currentIndexChanged(int index);
    void on_exitApp_clicked();
    void on_pushButton_clicked();

    void display_sensor_values();
    void display_connection_timeout_box();

    void RePaintCameraViewPainter() {_cameraViewPainter.repaint();}

    void on_actionExit_triggered();

private:
    // GUI related
    Ui::MainWindow *ui;
//    QTimer _timer_t0;
    QTimer _timer_t1;
//    QTimer _timer_t2;

    // Function related
    CrazyRadio _crazyRadio;
    Crazyflie _crazyFlie;
    CrazyFlieCaller _crazyFlieCaller;
    CameraViewPainter _cameraViewPainter;
};

#endif // MAINWINDOW_H

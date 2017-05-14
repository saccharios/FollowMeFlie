#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_activateCamera_clicked(bool checked);

    void on_disconnect_clicked(bool checked);

    void on_connect_clicked(bool checked);

    void on_radioSettingsOptions_currentIndexChanged(int index);

    void on_exitApp_clicked();

    void on_radioSettingsOptions_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

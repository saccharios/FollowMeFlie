#pragma once
#include <QWidget>

namespace Ui {
class ActualValuesWindow;
}

class ActualValuesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ActualValuesWindow(QWidget *parent = 0);
    ~ActualValuesWindow();

    void SetUp();

private:
    Ui::ActualValuesWindow *ui;
};

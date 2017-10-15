#pragma once
#include <QWidget>

namespace Ui {
class ParameterWindow;
}

class ParameterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterWindow(QWidget *parent = 0);
    ~ParameterWindow();

private:
    Ui::ParameterWindow *ui;
};


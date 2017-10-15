#include "parameter_window.h"
#include "ui_parameter_window.h"

ParameterWindow::ParameterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterWindow)
{
    ui->setupUi(this);
}

ParameterWindow::~ParameterWindow()
{
    delete ui;
}

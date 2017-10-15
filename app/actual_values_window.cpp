#include "actual_values_window.h"
#include "ui_actual_values_window.h"
#include <iostream>
ActualValuesWindow::ActualValuesWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActualValuesWindow)
{
    ui->setupUi(this);
}

ActualValuesWindow::~ActualValuesWindow()
{
    delete ui;
}


void ActualValuesWindow::SetUp()
{
    setWindowTitle("Actual Values");
    QPoint moveTo = {0,0}; // TODO SF Handle this nicely. ActualValuesWindows should be docked to the right or left side of the main window, or so.
    move(moveTo);


}

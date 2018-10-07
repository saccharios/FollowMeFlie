#include "set_point_dialog.h"
#include "ui_set_point_dialog.h"

SetPointDialog::SetPointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetPointDialog)
{
    ui->setupUi(this);
}

SetPointDialog::~SetPointDialog()
{
    delete ui;
}


void SetPointDialog::on_buttonBox_accepted()
{
    _setPoint.x = ui->setpoint_x_lineEdit->text().toFloat();
    _setPoint.y = ui->setpoint_y_lineEdit->text().toFloat();
    _setPoint.z = ui->setpoint_z_lineEdit->text().toFloat();

}

void SetPointDialog::on_buttonBox_rejected()
{

}

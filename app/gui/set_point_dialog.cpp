#include "set_point_dialog.h"
#include "ui_set_point_dialog.h"

SetPointDialog::SetPointDialog(Point3f setPoint, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetPointDialog),
    _setPoint(setPoint)
{
    ui->setupUi(this);

    ui->setpoint_x_lineEdit->setText(QString::number(_setPoint.x * 100.0f));
    ui->setpoint_y_lineEdit->setText(QString::number(_setPoint.y * 100.0f));
    ui->setpoint_z_lineEdit->setText(QString::number(_setPoint.z * 100.0f));
}

SetPointDialog::~SetPointDialog()
{
    delete ui;
}


void SetPointDialog::on_buttonBox_accepted()
{
    // TODO SF Validate input!
    _setPoint.x = ui->setpoint_x_lineEdit->text().toFloat()/100.0f;
    _setPoint.y = ui->setpoint_y_lineEdit->text().toFloat()/100.0f;
    _setPoint.z = ui->setpoint_z_lineEdit->text().toFloat()/100.0f;
    emit(NewSetPoint(_setPoint));

}

void SetPointDialog::on_buttonBox_rejected()
{

}

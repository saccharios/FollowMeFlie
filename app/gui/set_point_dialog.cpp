#include "set_point_dialog.h"
#include "ui_set_point_dialog.h"
#include <QPushButton>
#include <QIntValidator>

SetPointDialog::SetPointDialog(Point3f setPoint, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetPointDialog),
    _setPoint(setPoint)
{
    ui->setupUi(this);

    ui->setpoint_x_lineEdit->setText(QString::number(_setPoint.x * 100.0f));
    ui->setpoint_y_lineEdit->setText(QString::number(_setPoint.y * 100.0f));
    ui->setpoint_z_lineEdit->setText(QString::number(_setPoint.z * 100.0f));

    ui->setpoint_x_lineEdit->setValidator(new QIntValidator(20,100,this));
    ui->setpoint_y_lineEdit->setValidator(new QIntValidator(-100,100,this));
    ui->setpoint_z_lineEdit->setValidator(new QIntValidator(-100,100,this));
    Validate();
}

SetPointDialog::~SetPointDialog()
{
    delete ui;
}


void SetPointDialog::on_buttonBox_accepted()
{
    _setPoint.x = ReadX();
    _setPoint.y = ReadY();
    _setPoint.z = ReadZ();
    emit(NewSetPoint(_setPoint));

}

float SetPointDialog::ReadX()
{
    return ui->setpoint_x_lineEdit->text().toFloat()/100.0f;
}
float SetPointDialog::ReadY()
{
    return ui->setpoint_y_lineEdit->text().toFloat()/100.0f;
}
float SetPointDialog::ReadZ()
{
    return ui->setpoint_z_lineEdit->text().toFloat()/100.0f;
}
void SetPointDialog::on_buttonBox_rejected()
{

}

void SetPointDialog::Validate()
{
    float x_component = ReadX();
    _isValid =
            (0.199f <= x_component && x_component <= 1.0f) &&
            (std::abs(ReadY()) <= x_component/2.0f) &&
            (std::abs(ReadZ()) <= x_component/2.0f);

    SetValidText(_isValid);
    EnableOKButton(_isValid);
}
void SetPointDialog::SetValidText(bool isValid)
{
    if(isValid)
    {
        ui->valid_label->setText(QString("Input is valid"));
    }
    else
    {
        ui->valid_label->setText(QString("Input is NOT valid"));
    }
}

void SetPointDialog::on_setpoint_x_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    Validate();
}
void SetPointDialog::on_setpoint_y_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    Validate();
}
void SetPointDialog::on_setpoint_z_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    Validate();
}

void SetPointDialog::EnableOKButton(bool enable)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
}

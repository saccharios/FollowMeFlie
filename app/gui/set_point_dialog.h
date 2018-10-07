#pragma once
#include <QDialog>
#include "math/types.h"

namespace Ui {
class SetPointDialog;
}

class SetPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetPointDialog(Point3f setPoint, QWidget *parent = nullptr);
    ~SetPointDialog();

signals:
    void NewSetPoint(Point3f setPoint);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();


    void on_setpoint_x_lineEdit_textChanged(const QString &arg1);
    void on_setpoint_y_lineEdit_textChanged(const QString &arg1);
    void on_setpoint_z_lineEdit_textChanged(const QString &arg1);

private:
    Ui::SetPointDialog *ui;
    Point3f _setPoint = {};
    bool _isValid = false;

    float ReadX();
    float ReadY();
    float ReadZ();
    void Validate();
    void SetValidText(bool isValid);
    void EnableOKButton(bool enable);

};

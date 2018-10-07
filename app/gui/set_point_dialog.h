#ifndef SET_POINT_DIALOG_H
#define SET_POINT_DIALOG_H

#include <QDialog>
#include "math/types.h"

namespace Ui {
class SetPointDialog;
}

class SetPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetPointDialog(QWidget *parent = nullptr);
    ~SetPointDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SetPointDialog *ui;
    Point3f _setPoint = {};
};

#endif // SET_POINT_DIALOG_H

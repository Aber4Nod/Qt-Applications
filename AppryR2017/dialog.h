#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0, double *koef = NULL/*, void (*updateKoef) () = 0*/);
    ~Dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;

    double* logK=NULL;
//    void (*updateK)();
};

#endif // DIALOG_H

#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, double* koef/*, void (*updateKoef)()*/) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);


    logK = koef;
    ui->lineEdit->setText(QString("%1").arg(*logK));

    QString lineReg(QString("-?[0-9]*[.]?[0-9]+"));
    QRegExp regExp(lineReg);
    ui->lineEdit->setValidator(new QRegExpValidator(regExp,0));

    this->setWindowTitle("Параметр критерия");
//    updateK = updateKoef;
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    *logK = ui->lineEdit->text().toDouble();
    this->close();
//    updateK();
}

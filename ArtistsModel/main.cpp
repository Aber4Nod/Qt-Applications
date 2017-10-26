#include "mainwindow.h"
#include <QApplication>
#include"item.h"
#include"musicmodel.h"
#include<QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w ;


    w.show();
    return a.exec();
}

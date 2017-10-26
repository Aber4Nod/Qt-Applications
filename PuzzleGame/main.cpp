#include <QApplication>
#include "dialog.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLayout>
#include <QtWidgets>
#include "puzzlepiece.h"
#include "puzzlegame.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog d;
    d.show();

    return a.exec();
}

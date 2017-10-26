#ifndef DIALOGARTIST_H
#define DIALOGARTIST_H

#include <QDialog>
#include "musicmodel.h"
#include <QDataWidgetMapper>
#include "item.h"
namespace Ui {
class DialogArtist;
}

class DialogArtist: public QDialog
{
    Q_OBJECT

public:
    explicit DialogArtist(QWidget *parent = 0,Item *item=0);
    ~DialogArtist();
void setModel(QAbstractItemModel*);
void setModelIndex(const QModelIndex &);

private slots:

void on_pushButton_3_clicked();

void on_pushButton_clicked();

private:
    Ui::DialogArtist *ui;
    QDataWidgetMapper m_mapper;
    Artist* artist;
    QString fileName;
};

#endif // DIALOGARTIST_H

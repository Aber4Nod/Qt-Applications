#ifndef DIALOGALBUM_H
#define DIALOGALBUM_H

#include <QDialog>
#include "musicmodel.h"
#include <QDataWidgetMapper>
#include "item.h"
namespace Ui {
class dialogAlbum;
}

class dialogAlbum : public QDialog
{
    Q_OBJECT

public:
    explicit dialogAlbum(QWidget *parent = 0,Item *item=0);
    ~dialogAlbum();
void setModel(QAbstractItemModel*);
void setModelIndex(const QModelIndex &);

private slots:
void on_pushButton_2_clicked();

void on_pushButton_clicked();

//void documentWasModified();
private:
    Ui::dialogAlbum *ui;
    QDataWidgetMapper m_mapper;
    Album* album;
    QString fileName;
};

#endif // DIALOGALBUM_H

#ifndef INTERITEM_H
#define INTERITEM_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QtWidgets/QLineEdit>
#include <QFontMetrics>
#include <QItemDelegate>
#include <QDebug>
#include <QTableWidget>
#include <QRegExp>

class interItem : public QStyledItemDelegate
{
    Q_OBJECT
   public:
       explicit interItem(QObject *parent=0, bool *flag = 0, QPair<double,double> *edges = 0);
       QSize sizeHint(const QStyleOptionViewItem &option,  const QModelIndex &index) const override;
       void paint(QPainter *painter,  const QStyleOptionViewItem &option,  const QModelIndex &index) const override;

       QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option, const QModelIndex &index) const override;
       void setEditorData(QWidget *editor, const QModelIndex &index) const override;
       void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
       void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    private:
       bool *flagIn;
       QPair<double,double>* edges;
};

#endif // INTERITEM_H

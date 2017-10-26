#ifndef RATINGDELEGATE_H
#define RATINGDELEGATE_H

#include <QStyledItemDelegate>

class RatingDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit RatingDelegate(QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option,  const QModelIndex &index) const;
    void paint(QPainter *painter,  const QStyleOptionViewItem &option,  const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

signals:

public slots:

};

#endif // RATINGDELEGATE_H

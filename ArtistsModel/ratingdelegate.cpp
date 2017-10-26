#include "ratingdelegate.h"
#include <QPainter>
#include "item.h"
RatingDelegate::RatingDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QSize RatingDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(20, 1);
}

void RatingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    if (index.parent().parent().isValid()){
        int q=5;
        int r=static_cast<Song*>(index.internalPointer())->m_rating;
        if ((r<0)&&(r>5))
            return;
        painter->setRenderHint(QPainter::Antialiasing,true);

        painter->setBrush(QBrush(Qt::NoBrush));
        painter->setPen(Qt::black);

       for (int i=0;i<r;++i)
       {
            painter->setBrush(QBrush(Qt::green));
            painter->drawEllipse(QRect(option.rect.left()+2+20*i,option.rect.top()+2,16,16));
       }
        while(r<q){
            painter->setBrush(QBrush(Qt::NoBrush));
            painter->drawEllipse(QRect(option.rect.left()+2+20*r,option.rect.top()+2,16,16));
            r++;
        }
    }
}

QWidget *RatingDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   QSlider *slider = new QSlider(parent);
   slider->setRange(0,5);
   slider->setOrientation(Qt::Horizontal);
   slider->setAutoFillBackground(true);
   return slider;
}

void RatingDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QSlider *slider = qobject_cast<QSlider*>(editor);
    if(slider)
    slider->setValue(static_cast<Song*>(index.internalPointer())->m_rating);
}

void RatingDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QSlider *slider = qobject_cast<QSlider*>(editor);
    if(slider)
       slider->setGeometry(option.rect);

}

void RatingDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
   QSlider *slider = qobject_cast<QSlider*>(editor);
             if(slider)
                  model->setData(index,slider->value(),Qt::EditRole);
}

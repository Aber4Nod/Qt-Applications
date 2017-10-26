#include "interitem.h"

interItem::interItem(QObject *parent, bool *flag, QPair<double, double> *edges)
{
    flagIn=flag;
    this->edges = edges;
}

QSize interItem::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate *delegate = new QItemDelegate();
    return delegate->sizeHint(option,index);
}

void interItem::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate delegate;
    delegate.paint(painter,option,index);
}

QWidget *interItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit* editor_LE = new QLineEdit(parent);
    if (index.column()==1) {
        QString str(QString("[-]?[0-9]*[.]?[0-9]*;[-]?[0-9]*[.]?[0-9]*"));
        QRegExp regExp(str);
        QValidator* validator = new QRegExpValidator( regExp );
        editor_LE->setValidator(validator);
    }
    return editor_LE;
}

void interItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit* editor_LE = qobject_cast<QLineEdit*>(editor);
    *flagIn=false;

    editor_LE->setText(index.data().toString());
}

void interItem::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit* editor_LE = qobject_cast<QLineEdit*>(editor);
    if(editor_LE)
        editor_LE->setGeometry(option.rect);
}

void interItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column()==1) {
        QLineEdit *editor_LE = qobject_cast<QLineEdit*>(editor);
        QString str = editor_LE->text();
        QRegExp rx(QString("([-]?[0-9]*[.]?[0-9]*);([-]?[0-9]*[.]?[0-9]*)"));
        int pos = rx.indexIn(str);

        if (pos > -1)
        {
            double value;
            if (rx.cap(1)!="")
            {
                if (rx.cap(1)==".") {
                    value = 0;
                } else {
                    value = rx.cap(1).toDouble();
                }
                if (value<edges->first || value>edges->second)
                    return;
            }
            if (rx.cap(2)!="")
            {
                if (rx.cap(2)==".") {
                    value = 0;
                } else {
                    value = rx.cap(2).toDouble();
                }
                if (value<edges->first || value>edges->second)
                    return;
            }
        }

        model->setData(index,str,Qt::EditRole);
    }
}


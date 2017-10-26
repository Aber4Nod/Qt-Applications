#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPainter>
#include <QAbstractButton>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *e);

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *e);


    QBrush brush;
    QPixmap image1;
    QPoint pt;
    QPointF p1x[7],p2x[7];
    QPointF p1y[7],p2y[7];
    QPointF ps;
    QString fileName;
    bool pressed;
    bool pressed_left;
    bool brush_flag;
    bool resize_flag;

    int buf_x_,buf_y_;
    int buf_x,buf_y;
    int counter=1;
    QSizeF size_1,size_s;
    ~Dialog();
private slots:
 void on_pushButton_clicked();

 void on_pushButton_3_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H

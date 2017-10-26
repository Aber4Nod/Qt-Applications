#include "dialog.h"
#include <QtWidgets>
#include "ui_dialog.h"
#include <QPainter>
#include <QWidget>
#include "puzzlegame.h"
class MyWidget : public QWidget
{
public:
    MyWidget();

protected:
    void paintEvent(QPaintEvent *);
};

void Dialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::darkGreen);
    painter.drawRect(QRectF(pt,size_1));



    if((ps.x()<=pt.x()+size_1.width())&&(ps.y()<=pt.y()+size_1.height())&&(ps.x()>=pt.x())&&(ps.y())>=pt.y())
    {
        if (resize_flag==false){
            buf_x=1;
            buf_y=1;
            for (int i=0;i<7;i++) {
                if (p1x[i].x()<=ps.x())
                    buf_x++;
                if (p1y[i].y()<=ps.y())
                    buf_y++;
            }
        }
        if ((buf_x!=0)&&(buf_y!=0))
        {

            buf_x_=buf_x;
            buf_y_=buf_y;
            QSizeF size_buf(buf_x*(p1x[1].x()-p1x[0].x()),buf_y*(p1y[1].y()-p1y[0].y()));
            size_s=size_buf;


            if (brush_flag==false) {
                painter.setBrush(QBrush(Qt::yellow));
            }
            else {
                painter.setBrush(brush);
                // painter.scale(size_s.width(),size_s.height());
                QSize size1(size_s.width(),size_s.height());
                QPixmap image2= image1.scaled(size1);
                brush=QBrush(image2);
                brush.setStyle(Qt::TexturePattern);
                painter.setBrush(brush);
            }






            painter.drawRect(QRectF(pt,size_s));

        }


    }
    if (resize_flag==true){
        if (counter==1){
            buf_x_=0;
            buf_y_=0;
            counter++;
        }
        QSizeF size_buf(buf_x_*(p1x[1].x()-p1x[0].x()),buf_y_*(p1y[1].y()-p1y[0].y()));
        size_s=size_buf;


        if (brush_flag==false) {
            painter.setBrush(QBrush(Qt::yellow));
        }
        else {
            painter.setBrush(brush);
            QSize size1(size_s.width()+0,size_s.height());
            QPixmap image2= image1.scaled(size1);
            brush=QBrush(image2);
            brush.setStyle(Qt::TexturePattern);
            painter.setBrush(brush);
        }
        painter.drawRect(QRectF(pt,size_s));
    }

    for (int i=0;i<7;i++) {
        painter.drawLine(p1x[i],p2x[i]);
        painter.drawLine(p1y[i],p2y[i]);
    }

    resize_flag=true;
}

void Dialog::resizeEvent(QResizeEvent* e)
{
    QSizeF size_3(e->size().width()-ui->label_2->size().width()-50,e->size().height()-2*(ui->pushButton->size().height()+1)-45);
    size_1=size_3;
    QSize size1(size_1.width()+0,size_1.height()+0);
    QPixmap image2=image1.scaled(size1,Qt::IgnoreAspectRatio);
    brush=QBrush(image2);


    for (int i=1;i<=7;i++) {
        double t = (e->size().width()-ui->label_2->size().width()-50);
        double r = (e->size().height()-2*(ui->pushButton->size().height()+1)-45);
    QPointF px11(i*(t/8)+pt.x(),pt.y());
    QPointF px12(i*(t/8)+pt.x(),pt.y()+r);
    QPointF py11(pt.x(),pt.y()+i*r/8);
    QPointF py12(t+pt.x(),pt.y()+i*r/8);
    p1x[i-1]=px11;
    p2x[i-1]=px12;
    p1y[i-1]=py11;
    p2y[i-1]=py12;
    }
    update();
}

void Dialog::mousePressEvent(QMouseEvent *e)
{
    QPointF p_buf(e->pos().x(),e->pos().y());
    ps=p_buf;
    qDebug()<<e->pos();
    update();
    pressed=true;
    resize_flag=false;
    if (e->button()==Qt::LeftButton)
    {
        pressed_left=true;
    }

}

void Dialog::mouseReleaseEvent(QMouseEvent *)
{
    if (pressed_left==true)
      resize_flag=false;
    pressed=false;
    pressed_left=false;
    update();
}

void Dialog::mouseMoveEvent(QMouseEvent *e)
{

    if ((pressed_left==true))
    {
        resize_flag=false;
        QPointF p_buf(e->pos().x(),e->pos().y());
        ps=p_buf;
        qDebug()<<e->pos();
        update();
    }

}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    fileName="";
    brush_flag=false;
    buf_x=1,buf_y=1;
    QPoint pt_1(ui->label_2->size().width(),ui->pushButton->size().height()+15);
    pt=pt_1;

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }
        ui->lineEdit->setText(fileName);
        QPixmap im_;
        im_.convertFromImage(image,Qt::AutoColor);
        QSize size7(size_1.width(),size_1.height());

        image1=im_.scaled(size7);

        brush_flag=true;

        update();
    }
}

void Dialog::on_pushButton_3_clicked()
{
    if ((buf_x_>1)&&(buf_y_>1)){
        QGraphicsView  *gv = new QGraphicsView();
        QGraphicsScene gs;
        PuzzleGame *game = new PuzzleGame(NULL,buf_x_,buf_y_);
        if (fileName.isEmpty()){
            QImage image(QSize(buf_x_*100,buf_y_*100), QImage::Format_RGB32);
            image.fill(qRgb(255,255,0));
            game->image1.convertFromImage(image);
        }
        else
            game->image1.load(fileName);
        game->setup();
        gv->setScene(&game->scene);
        gv->show();
        this->hide();
    }
    else {
        if ((buf_x_<=1)&&(buf_y_>1))
            QMessageBox::information(this, tr("Error"),
                                     tr("Enter more colums"));
        if ((buf_x_>1)&&(buf_y_<=1))
            QMessageBox::information(this, tr("Error"),
                                     tr("Enter more rows"));
        if ((buf_x_<=1)&&(buf_y_<=1))
            QMessageBox::information(this, tr("Error"),
                                     tr("Enter more columns and rows"));

    }
}

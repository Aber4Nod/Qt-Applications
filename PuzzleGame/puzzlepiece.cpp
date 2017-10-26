#include "puzzlepiece.h"
#include<QPainter>
#include <QSet>
#include "puzzlegame.h"
#include "QMessageBox"
PuzzlePiece::PuzzlePiece(QGraphicsItem *parent) :
    QGraphicsPathItem(parent)
{
}
PuzzlePiece::PuzzlePiece(PuzzlePiece::ConnectorPosition north, PuzzlePiece::ConnectorPosition south, PuzzlePiece::ConnectorPosition west, PuzzlePiece::ConnectorPosition east,int count_el)
{
    numb_el=count_el;
    con_pos[North]=north;//по умолчанию коннекторов нет
    con_pos[South]=south;
    con_pos[West]=west;
    con_pos[East]=east;
    size = PIECE_SIZE;

    constructPath(sh);
    setPath(sh);
    m_coordinates=QPoint(0,0);
    m_neighbours[North]=0;//и так для всех 4
    m_neighbours[South]=0;
    m_neighbours[West]=0;
    m_neighbours[East]=0;
}
void PuzzlePiece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QVector<QPoint> vec;


    checkNeighbors(vec);
    if (vec.count()==numb_el)
    {
        QMessageBox msgBox;
        msgBox.setText("Congratulations, you have collected a puzzle!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        if(ret==QMessageBox::Ok)
            exit(0);
    }
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
void PuzzlePiece::checkNeighbors(QVector<QPoint> &checked)
{
    if (checked.contains(coordinates()))
    {
       return;
    }
    checked.insert(checked.begin(),coordinates());
    findneighbor(PuzzlePiece::North);
    findneighbor(PuzzlePiece::South);
    findneighbor(PuzzlePiece::East);
    findneighbor(PuzzlePiece::West);

    if (m_neighbours[North])
        m_neighbours[North]->checkNeighbors(checked);
    if (m_neighbours[South])
        m_neighbours[South]->checkNeighbors(checked);
    if (m_neighbours[West])
        m_neighbours[West]->checkNeighbors(checked);
    if (m_neighbours[East])
        m_neighbours[East]->checkNeighbors(checked);

}
QVariant PuzzlePiece::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change==ItemPositionHasChanged && scene())
    {
        QPointF newPos=value.toPointF();
        if (m_neighbours[North])
            m_neighbours[North]->setPos(newPos.x(),newPos.y()-100);
        if (m_neighbours[South])
            m_neighbours[South]->setPos(newPos.x(),newPos.y()+100);
        if (m_neighbours[West])
            m_neighbours[West]->setPos(newPos.x()-100,newPos.y());
        if (m_neighbours[East])
            m_neighbours[East]->setPos(newPos.x()+100,newPos.y());
    }
         return QGraphicsItem::itemChange(change, value);
}
void PuzzlePiece::findneighbor(PuzzlePiece::Direction direct)
{
    PuzzlePiece * n_piece;
    if (m_neighbours[direct])
        return;
    if (direct==PuzzlePiece::North)
    {
        double buf_pos_x = pos().x();
        double buf_pos_y = pos().y();
        for (double i=buf_pos_x-1;i<=buf_pos_x+1;i+=1)
        {
            for (double j=buf_pos_y-101;j<=buf_pos_y-99;j+=1)
                if (n_piece=dynamic_cast<PuzzlePiece*>(scene()->itemAt(i,j,QTransform())))
                {
                    if ((n_piece->coord.x()==coord.x())&&((n_piece->coord.y()==coord.y()-1)))
                           {
                            m_neighbours[direct]=n_piece;
                            n_piece->m_neighbours[South]=this;
                            n_piece->setPos(pos().x(),pos().y()-100);
                           }
                }
        }
    }
    if (direct==PuzzlePiece::South)
    {
        double buf_pos_x = pos().x();
        double buf_pos_y = pos().y();
        for (double i=buf_pos_x-1;i<=buf_pos_x+1;i+=1)
        {
            for (double j=buf_pos_y+99;j<=buf_pos_y+101;j+=1)
                if (n_piece=dynamic_cast<PuzzlePiece*>(scene()->itemAt(i,j,QTransform())))
                {
                    if ((n_piece->coord.x()==coord.x())&&((n_piece->coord.y()==coord.y()+1)))
                           {
                            m_neighbours[direct]=n_piece;
                            n_piece->m_neighbours[North]=this;
                            n_piece->setPos(pos().x(),pos().y()+100);
                           }
                }
        }
    }
    if (direct==PuzzlePiece::West)
    {
        double buf_pos_x = pos().x();
        double buf_pos_y = pos().y();
        for (double i=buf_pos_x-101;i<=buf_pos_x-99;i+=1)
        {
            for (double j=buf_pos_y-1;j<=buf_pos_y+1;j+=1)
                if (n_piece=dynamic_cast<PuzzlePiece*>(scene()->itemAt(i,j,QTransform())))
                {
                    if ((n_piece->coord.x()==coord.x()-1)&&((n_piece->coord.y()==coord.y())))
                           {
                            m_neighbours[direct]=n_piece;
                            n_piece->m_neighbours[East]=this;
                            n_piece->setPos(pos().x()-100,pos().y());
                           }
                }
        }
    }
    if (direct==PuzzlePiece::East)
    {
        double buf_pos_x = pos().x();
        double buf_pos_y = pos().y();
        for (double i=buf_pos_x+99;i<=buf_pos_x+101;i+=1)
        {
            for (double j=buf_pos_y-1;j<=buf_pos_y+1;j+=1)
                if (n_piece=dynamic_cast<PuzzlePiece*>(scene()->itemAt(i,j,QTransform())))
                {
                    if ((n_piece->coord.x()==coord.x()+1)&&((n_piece->coord.y()==coord.y())))
                           {
                            m_neighbours[direct]=n_piece;
                            n_piece->m_neighbours[West]=this;
                            n_piece->setPos(pos().x()+100,pos().y());
                           }
                }
        }
    }
}
QPoint PuzzlePiece::coordinates()
{
    return coord;
}
void PuzzlePiece::setCoordinates( QPoint &point)
{
    if (coord==point)
        return;
    coord=point;
}

void PuzzlePiece::constructPath(QPainterPath &p)
{
    p.moveTo(-1*size/2., -1*size/2.);
    //north side
    p.lineTo(-1*size/4., -1*size/2.);
    if  (con_pos[North] == Out) p.arcTo(-1*size/4., -3*size/4.,1*size/2., 1*size/2.,180, -180);
    if (con_pos[North] == In)  p.arcTo(-1*size/4., -3*size/4.,1*size/2., 1*size/2.,180, 180);
    if (con_pos[North] == Non) p.lineTo(size/4., -1*size/2.);
    p.lineTo(size/2., -1*size/2.);
    //east side
     p.lineTo(size/2., -1*size/4.);
     if (con_pos[East] == Out) p.arcTo(size/4., -size/4.,1*size/2., 1*size/2.,90, -180);
     if (con_pos[East] == In)  p.arcTo(size/4., -size/4.,1*size/2., 1*size/2.,90, 180);
     if (con_pos[East] == Non) p.lineTo(size/2., size/4.);
     p.lineTo(size/2., size/2.);
     //south side
     p.lineTo(size/4., size/2.);
     if (con_pos[South] == Out) p.arcTo(-size/4., size/4.,1*size/2., 1*size/2.,0, -180);
     if (con_pos[South] == In)  p.arcTo(-size/4., size/4.,1*size/2., 1*size/2.,0, 180);
     if (con_pos[South] == Non)
     {
         p.lineTo(-size/4., size/2.);
     }
         p.lineTo(-1*size/2., size/2.);
    //west side
     p.lineTo(-1*size/2., size/4.);
     if (con_pos[West] == Out) p.arcTo(-3*size/4., -size/4.,1*size/2., 1*size/2.,270, -180);
     if (con_pos[West] == In)  p.arcTo(-3*size/4., -size/4.,1*size/2., 1*size/2.,270, 180);
     if (con_pos[West] == Non)
     {
         p.lineTo(-1*size/2., -1*size/4.);
     }
     p.lineTo(-1*size/2., -1*size/2.);
    //замыкаем путь
    p.closeSubpath();
}

void PuzzlePiece::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setClipPath(path());
    QRect rect=boundingRect().toRect();
    painter->drawPixmap(rect.x(),rect.y(), m_image);
    painter->setPen(Qt::black);
    painter->drawPath(path());
    update();
}

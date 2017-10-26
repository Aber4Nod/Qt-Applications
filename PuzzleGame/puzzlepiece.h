#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QGraphicsPathItem>
#include <QGraphicsPathItem>
#include<QObject>
#include<QPixmap>
#include<QPoint>

#define PIECE_SIZE 100
class PuzzlePiece : public QGraphicsPathItem
{
    //QGraphicsItem
public:
    explicit PuzzlePiece(QGraphicsItem *parent = 0);
    enum ConnectorPosition {Non, In, Out};
    enum Direction {North, South, West, East};
    PuzzlePiece(ConnectorPosition north, ConnectorPosition south, ConnectorPosition west, ConnectorPosition east, int count_el);
    void constructPath(QPainterPath &p);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPixmap m_image;
    QPoint coordinates();
    void setCoordinates( QPoint & point);
    void findneighbor(PuzzlePiece::Direction direct);
    void checkNeighbors(QVector<QPoint> &checked);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    PuzzlePiece* m_neighbours[4];
    QPoint coord;

private:
    int numb_el;
    ConnectorPosition con_pos[4];
    qreal size;
    QPainterPath sh;

    QPoint m_point;
    QPoint m_coordinates;

signals:

public slots:

};

#endif // PUZZLEPIECE_H

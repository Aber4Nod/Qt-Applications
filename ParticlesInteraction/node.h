#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>
#include <graphwidget.h>

void calculateForces(std::list<uint32_t> *numbers, std::map<std::pair<uint32_t,uint32_t>*, std::pair<double, double> *> &dataPoints, const double &radius,
                 void (*callback)(std::map<uint32_t, std::pair<double, double> *> *));

class Node : public QGraphicsItem
{
public:
    Node(GraphWidget *graphWidget);
    bool advance();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    uint32_t number;
    QPointF newPos;
    bool jCreated = false;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    GraphWidget *graph;
};

#endif // NODE_H

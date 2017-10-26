#include "node.h"
#include "graphwidget.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <map>
#include <iterator>
#include <limits.h>
void calculateForces(std::list<uint32_t> *numbers, std::map<std::pair<uint32_t, uint32_t> *, std::pair<double, double> *> &dataPoints, const double &radius,
                           void (*callback)(std::map<uint32_t, std::pair<double, double> *>*))
{
    std::map<std::pair<uint32_t, uint32_t>*, std::pair<double,double>*>::iterator item;
    std::list<uint32_t>::iterator item_l;
    std::map<uint32_t, std::pair<double, double> *>* retVal = new std::map<uint32_t, std::pair<double, double> *>();
    for (item_l=numbers->begin();item_l!=numbers->end();++item_l) {
        item = dataPoints.begin();
        double xvel=0;
        double yvel=0;
        for (;item!=dataPoints.end();++item) {
            if ((*item).first->first != *item_l) {
                double dx = item->second->first;
                double dy = item->second->second;
                double l = sqrt(dx * dx + dy * dy);
                double lNom = l  - 2*radius;
                if (l > pow(10,-3)) {
                    xvel += dx*((1/lNom/lNom) - (1/lNom))/l;
                    yvel += dy*((1/lNom/lNom) - (1/lNom))/l;
                }
            }
        }
        std::pair<double,double>* P = new std::pair<double, double>(xvel/*/radius*/,yvel/*/radius*/);
        std::pair<uint32_t, std::pair<double,double>*>* R = new std::pair<uint32_t, std::pair<double,double>*>(*item_l,P);
        retVal->insert(retVal->end(), *R);
    }
    callback(retVal);
}

Node::Node(GraphWidget *graphWidget)
    : graph(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

bool Node::advance()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}

QRectF Node::boundingRect() const
{
    qreal adjust = 0;
    return QRectF( -5 - adjust, -5 - adjust, 10 + adjust, 10 + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-5, -5, 10, 10);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-5, -5, 10, 10);

    QRadialGradient gradient(-1.5, -1.5, 5);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(1.5, 1.5);
        gradient.setFocalPoint(1.5, 1.5);
        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
    } else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-5, -5, 10, 10);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button()==Qt::RightButton && !jCreated) {
        delete this;
    }
    else {
        update();
        QGraphicsItem::mousePressEvent(event);
    }
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    jCreated = false;
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}


#include "graphwidget.h"
#include "node.h"
#include <math.h>
#include <QKeyEvent>
#include <QtConcurrent/QtConcurrent>

bool calced = false;
std::map<uint32_t,std::pair<double, double>*> ret;

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(0, 0, 700, 700);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(400, 400);
    setWindowTitle(tr("Particles interaction"));

    for (int i=0;i<15;++i)
    {
        Node* node = new Node(this);
        size++;
        node->number = i;
        scene->addItem(node);
        node->setPos(qrand() % 700, qrand() % 700);
        if (!radius) {
            radius=node->boundingRect().width()/2;
        }
    }
}


void GraphWidget::callback(std::map<uint32_t, std::pair<double, double> *> *retVal)
{
    ret = *retVal;
    calced=true;
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        break;
    case Qt::Key_Down:
        break;
    case Qt::Key_Left:
        break;
    case Qt::Key_Right:
        break;
    case Qt::Key_Plus:
        break;
    case Qt::Key_Minus:
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphWidget::timerEvent(QTimerEvent *)
{
    if (calced) {
        calced=false;
        calNewPos();
        updTimer();
    }
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

}

bool GraphWidget::event(QEvent *event)
{
    if (stated) {
        if (scene()) {
            if (scene()->items().size()) {
                stated=false;
                updTimer();
            }
        }
    }
    return QWidget::event(event);
}

void GraphWidget::calNewPos()
{
    std::map<uint32_t,std::pair<double, double>*>::iterator iter;
    foreach (QGraphicsItem *item, scene()->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        iter = ret.begin();
        for (;iter!=ret.end(); ++iter) {
            if (node->number==(*iter).first){
                QRectF sceneRect = scene()->sceneRect();
                if (node->scene()->mouseGrabberItem() != node)
                    node->newPos = node->pos() + QPointF((*iter).second->first, (*iter).second->second)/(/*radius+*/scene()->items().size());
                else
                    node->newPos = node->pos();
                node->newPos.setX(qMin(qMax(node->newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
                node->newPos.setY(qMin(qMax(node->newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
            }
        }
    }

    foreach (QGraphicsItem *item, scene()->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        node->advance();
    }
}

void GraphWidget::calNewPositions()
{
    std::map<std::pair<uint32_t,uint32_t>*, std::pair<double,double>*>* dataPoints =
            new std::map<std::pair<uint32_t,uint32_t>*, std::pair<double,double>*>();
    QList<Node *> nodes;
    std::list<uint32_t>* numbers = new std::list<uint32_t>();

    foreach (QGraphicsItem* item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node*>(item)) {
            nodes<<node;
        }
    }

    foreach (Node *node, nodes) {
        foreach (QGraphicsItem* item, scene()->items()) {
            Node *nodeIN = qgraphicsitem_cast<Node *>(item);
            if (!nodeIN)
                continue;
            QPointF vec = node->mapToItem(nodeIN, 0, 0);
            std::pair<uint32_t,uint32_t>* pair_o = new std::pair<uint32_t,uint32_t>(nodeIN->number,node->number);
            std::pair<double,double>* pair = new std::pair<double,double>(vec.x(),vec.y());
            dataPoints->insert(dataPoints->end(),std::make_pair(pair_o,pair));
        }
        numbers->insert(numbers->end(), node->number);
    }
    QtConcurrent::run(calculateForces, numbers, *dataPoints, radius, &callback);
}

void GraphWidget::updTimer()
{
    calNewPositions();
    if (timer)
        killTimer(timer);
    timer = startTimer(1);
}

void GraphWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button()==Qt::RightButton) {
        QPointF point(e->pos());
        if (!scene()->itemAt(e->pos(), QTransform())) {
            Node* node = new Node(this);
            node->number = ++size;
            node->newPos = point;
            node->jCreated = true;
            scene()->addItem(node);
            node->setPos(point);
        }
    }
    QGraphicsView::mousePressEvent(e);
}

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QThread>
#include <QGraphicsItem>
#include <QFuture>
#include <QTimer>
#include <QEventLoop>
class Node;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = 0);
    static void callback(std::map<uint32_t, std::pair<double, double> *> *retVal);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void calculation();

private:
    int timerId;
    Node *centerNode;
    bool event(QEvent* event) override;
    uint32_t size=0;
    QPointF newP;
    double radius=0.;
    bool stated = true;
    void calNewPos();
    void calNewPositions();
    bool conc=false;
    uint64_t timer=0;

private slots:
    void updTimer();
signals:
    void calculated();
};

#endif // GRAPHWIDGET_H
